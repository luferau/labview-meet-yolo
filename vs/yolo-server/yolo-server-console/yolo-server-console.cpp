// server_test_console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <json/json.h>
#include <iostream>
#include <opencv2/opencv.hpp>            // C++

#include "../base64/base64.h"
#include "include/crow.h"

int main()
{
    std::cout << "yolo-server-console running...\n";

    crow::SimpleApp app;

    CROW_ROUTE(app, "/version")
        ([]{
            std::string response = "opencv:" + cv::getVersionString();

            crow::json::wvalue x;
            x["message"] = response;
            
            return x;
        });

    CROW_ROUTE(app, "/detect").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req, crow::response& res) {


        std::string body = req.body;

        // base64 to cv::Mat
        std::string dec_jpg = base64_decode(body);
        std::vector<uchar> data(dec_jpg.begin(), dec_jpg.end());
        cv::Mat image_mat = cv::imdecode(cv::Mat(data), 1);
        std::cout << "Height: " << image_mat.rows << " Width: " << image_mat.cols << "\n";
        cv::imwrite("received.jpg", image_mat);

        std::ostringstream os;
        os << "done";

        res.write(os.str());
        res.end();
        });

    app.port(18080).multithreaded().run();

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
