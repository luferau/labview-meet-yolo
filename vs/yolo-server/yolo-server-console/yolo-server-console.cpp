// server_test_console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <opencv2/opencv.hpp>

#include "../include/yolo_v2_class.hpp"
#include "../include/base64/base64.cpp"
#include "../include/crow.h"

std::vector<std::string> objects_names_from_file(std::string const filename) {
    std::ifstream file(filename);
    std::vector<std::string> file_lines;
    if (!file.is_open()) return file_lines;
    for (std::string line; getline(file, line);) file_lines.push_back(line);
    std::cout << "object names loaded \n";
    return file_lines;
}

std::string show_result(std::vector<bbox_t> const result_vec, std::vector<std::string> const obj_names, int frame_id = -1) {
    std::ostringstream os;
    if (frame_id >= 0) os << " Frame: " << frame_id << std::endl;
    for (auto& i : result_vec) {
        if (obj_names.size() > i.obj_id) os << obj_names[i.obj_id] << " - ";
        os << "obj_id = " << i.obj_id << ",  x = " << i.x << ", y = " << i.y
            << ", w = " << i.w << ", h = " << i.h
            << std::setprecision(3) << ", prob = " << i.prob << std::endl;
    }
    return os.str();
}

int main()
{
    std::cout << "yolo-server-console running...\n";

    // crow server
    crow::SimpleApp app;
    
    // yolo detector
    static std::shared_ptr<Detector> detector;
    static std::vector<std::string> obj_names;

    // endpoints
    CROW_ROUTE(app, "/version")
        ([&]{
            std::string response = "opencv:" + cv::getVersionString();

            crow::json::wvalue x;
            x["version"] = response;
            return x;
        });

    CROW_ROUTE(app, "/init")
        ([&] (const crow::request& req) {
            std::string  names_file = req.url_params.get("names");
            std::string  cfg_file = req.url_params.get("cfg");
            std::string  weights_file = req.url_params.get("weights");;

            // response
            std::ostringstream response_stream;
        
            try 
            {
                detector = std::make_shared<Detector>(cfg_file, weights_file);
                obj_names = objects_names_from_file(names_file);

                response_stream << "ok";
            }
            catch (std::exception& e) { response_stream << "exception: " << e.what(); }
            catch (...) { response_stream << "unknown exception"; }

            crow::json::wvalue response;
            response["result"] = response_stream.str();

            return response;
        });

    CROW_ROUTE(app, "/dispose")
        ([&] {
            // response
            std::ostringstream response_stream;

            try
            {
                detector->~Detector();

                response_stream << "ok";
            }
            catch (std::exception& e) { response_stream << "exception: " << e.what(); }
            catch (...) { response_stream << "unknown exception"; }

            crow::json::wvalue response;
            response["result"] = response_stream.str();

            return response;
        });

    CROW_ROUTE(app, "/detect").methods(crow::HTTPMethod::Post)
        ([&](const crow::request& req) {
            // response
            std::ostringstream response_stream;
            std::string result = "";

            try
            {
                std::string body = req.body;

                // base64 to cv::Mat
                std::string image_decoded = base64_decode(body, true);
                std::vector<uchar> image_data(image_decoded.begin(), image_decoded.end());
                cv::Mat image_mat = cv::imdecode(cv::Mat(image_data), 1);

                std::cout << "Image in processing: " << "Height: " << image_mat.rows << " Width: " << image_mat.cols << " Channels count: " << image_mat.channels() << "\n";

                // save to file
                // cv::imwrite("received.png", image_mat);

                // detecting 
                std::vector<bbox_t> result_vec = detector->detect(image_mat);
                result = show_result(result_vec, obj_names);

                response_stream << "ok";
            }
            catch (std::exception& e) { response_stream << "exception: " << e.what(); }
            catch (...) { response_stream << "unknown exception"; }

            crow::json::wvalue response;
            response["result"] = response_stream.str();
            response["objects"] = result;

            return response;
        });

    // app.port(18080).multithreaded().run();
    app.port(18080).run();

    system("pause");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
