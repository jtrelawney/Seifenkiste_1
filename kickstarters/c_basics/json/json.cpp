#include <iostream>
#include <iomanip>
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

int main()
{


    std::ifstream in("./test.json");
    json file = json::parse(in);


        /*
    try {
        auto& array = file.at("usb_camera");

        for (auto&& val: array) {
                std::cout << val << std::endl;
            }
        } catch(std::exception&) {
            std::cout << "no luck" << std::endl;
        }
        */

    auto& camera = file.at("usb_camera");

    for (auto&& val: camera) {
            std::cout << val << std::endl;
        }


    for ( auto camera: file["usb_camera"].items() )
    {
        std::cout << camera.key() << " = " << camera.value() << "\n";
    }

    int w = camera["width"];
    float h = camera["height"];
    std::cout << "camera width, height = " << w << " , " << h << " = " << w+h << "\n";


    for (auto usonic = file["usonic"].begin(); usonic != file["usonic"].end(); ++usonic)
    {
        std::cout << usonic.key() << " = " << usonic.value() << "\n";
    }


    /*
    // create a JSON object
    json j =
    {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {
            "answer", {
                {"everything", 42}
            }
        },
        {"list", {1, 0, 2}},
        {
            "object", {
                {"currency", "USD"},
                {"value", 42.99}
            }
        }
    };

    // add new values
    j["new"]["key"]["value"] = {"another", "list"};

    // count elements
    auto s = j.size();
    j["size"] = s;

    // pretty print with indent of 4 spaces
    std::cout << std::setw(4) << j << '\n';
*/
}
