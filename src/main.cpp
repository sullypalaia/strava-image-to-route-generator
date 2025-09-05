#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

int main() {
    cv::Mat img = cv::imread("resources/examples/smiley-face3.png"); //add path to image here
    if (img.empty()) {
        std::cout << "failed to open window\n";
        return -1;
    }

    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(img, img, cv::Size(7, 7), 1, 0);
    cv::Canny(img, img, 80, 100);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(img, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    std::ofstream file_out("route.gpx", std::ios::out, std::ios::trunc);
    if (file_out.is_open()) {
        file_out << 
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<gpx xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\" creator=\"cool route\" version=\"1.1\" xmlns=\"http://www.topografix.com/GPX/1/1\">\n\
    <metadata>\n\
        <time>2025-09-01T20:00:00Z</time>\n\
    </metadata>\n\
    <trk>\n\
        <name>my route</name>\n\
        <type>other</type>\n\
        <trkseg>\n";
//change value of "creator" if duplicate
//can also change time if needed; times may conflict and result in error when uploading

            int secs = 0;
            int mins = 0;
            int hrs = 0;
            int t = 0;

        for (size_t i = 0; i < contours.size(); i++) {
            for (size_t j = 0; j < contours[i].size(); j++) {
                cv::Point p = contours[i][j];

                double lon = ((p.x / double(img.cols)) * 90.0) - 45.0;
                double lat = (90.0/4.0) - ((p.y / double(img.rows)) * 45.0);

                if (t != 0) {
                    if (t % 60 == 0) {
                        secs = 0;
                        ++mins;
                    }
                    else {
                        ++secs;
                    }
                    
                    if (t % 3600 == 0) {
                        mins = 0;
                        ++hrs;
                    }
                }

                std::array<int, 3> time = {secs, mins, hrs};
                std::string secs_s, mins_s, hrs_s;
                std::array<std::string, 3> time_s = {secs_s, mins_s, hrs_s};
                for (int z = 0; z < time.size(); z++) {
                    time_s[z] = time[z] < 10 ? "0" + std::to_string(time[z]) : std::to_string(time[z]);
                }
                
                file_out << "           <trkpt lat=\"" << lat << "\" lon=\"" << lon << "\">\n"
                        << "                <ele>50.0</ele>\n"
                        << "                <time>2025-09-01T" << time_s[2] << ":" << time_s[1] << ":" << time_s[0] << "Z</time>\n"
                        << "            </trkpt>\n";
            
                ++t;
            }

            if (i == contours.size() - 1) {
                std::cout << "file creation complete\n";
            }
        }

        file_out << "       </trkseg>\n"
                << "    </trk>\n"
                << "</gpx>";
    }

    cv::waitKey(0);

    return 0;
}