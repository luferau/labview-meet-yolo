// server_test_console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <json/json.h>

#include "include/crow.h"
#include <iostream>

#include <opencv2/opencv.hpp>            // C++

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

    CROW_ROUTE(app, "/detect")
        ([](const crow::request& req, crow::response& res) {

        std::string body = req.body;



        std::vector<char> image_vector(body.begin(), body.end());
        cv::Mat image_mat(image_vector, true);
        cv::Mat image(cv::imdecode(image_mat, 1));

        std::cout << "Height: " << image.rows << " Width: " << image.cols << "\n";

        cv::imwrite("received.jpg", image);

        //replace cat.jpg with your file path
        //res.set_static_file_info("cat.jpg");
        //res.end();
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
