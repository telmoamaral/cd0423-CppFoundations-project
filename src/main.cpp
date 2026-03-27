#include <optional>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cairo/cairo.h>
#include "io2d.h"
#include "route_model.h"
#include "render.h"
#include "route_planner.h"

using namespace std::experimental;

static std::optional<std::vector<std::byte>> ReadFile(const std::string &path)
{   
    std::ifstream is{path, std::ios::binary | std::ios::ate};
    if( !is )
        return std::nullopt;
    
    auto size = is.tellg();
    std::vector<std::byte> contents(size);    
    
    is.seekg(0);
    is.read((char*)contents.data(), size);

    if( contents.empty() )
        return std::nullopt;
    return std::move(contents);
}

int main(int argc, const char **argv)
{    
    std::string osm_data_file = "";
    bool use_test_coords = false;
    if( argc > 1 ) {
        for( int i = 1; i < argc; ++i ){
            if( std::string_view{argv[i]} == "-f" && ++i < argc )
                osm_data_file = argv[i];
            if( std::string_view{argv[i]} == "-t")
                use_test_coords = true;
        }
    }
    if (osm_data_file == "") {
        std::cout << "To specify a map file use the following format: " << std::endl;
        std::cout << "Usage: [executable] [-f filename.osm]" << std::endl;
        osm_data_file = "../map.osm";
    }
    
    std::vector<std::byte> osm_data;
 
    if( osm_data.empty() && !osm_data_file.empty() ) {
        std::cout << "Reading OpenStreetMap data from the following file: " <<  osm_data_file << std::endl;
        auto data = ReadFile(osm_data_file);
        if( !data )
            std::cout << "Failed to read." << std::endl;
        else
            osm_data = std::move(*data);
    }
 
    // Build Model.
    RouteModel model{osm_data};

    // For testing:
    // RouteModel model{osm_data, 10, 10, 90, 90};

    // TODO 1: Declare floats `start_x`, `start_y`, `end_x`, and `end_y` and get
    // user input for these values using std::cin. Pass the user input to the
    // RoutePlanner object below in place of 10, 10, 90, 90.
    float start_x, start_y, end_x, end_y;
    if(use_test_coords){
        start_x = 10;
        start_y = 10;
        end_x = 90;
        end_y = 90;
    }
    else{
        std::cout << "Please enter a start x coordinate between 0 and 100: ";
        std::cin >> start_x;
        std::cout << "Please enter a start y coordinate between 0 and 100: ";
        std::cin >> start_y;
        std::cout << "Please enter an end x coordinate between 0 and 100: ";
        std::cin >> end_x;
        std::cout << "Please enter an end y coordinate between 0 and 100: ";
        std::cin >> end_y;
    }
    
    RoutePlanner route_planner{model, start_x, start_y, end_x, end_y};
    route_planner.AStarSearch();
    std::cout << "Distance: " << route_planner.GetDistance() << " meters. \n";
    
    // Create render object
    Render render{model};

    // Create an image surface
    auto surface = io2d::image_surface{io2d::format::argb32, 400, 400};
    
    // Render to the surface
    render.Display(surface);

    // Save the surface to a PNG file
    surface.save("map_routed.png", io2d::image_file_format::png);
    
    std::cout << "Route has been rendered to map_routed.png" << std::endl;
    
    return 0;
}
