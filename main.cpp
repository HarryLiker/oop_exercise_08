#include <iostream>
#include <thread>
#include <vector>
#include <functional>

#include "factory.hpp"
#include "figure.hpp"
#include "publish-subscribe.hpp"

PublishSubscribe<std::vector<std::shared_ptr<Figure>>> message_queue;
const std::string FILE_NAME = "file_test";
FILE *file = NULL;

void ThreadFunction() {
    using function = std::function<void(std::shared_ptr<Figure> figure)>;

    function PrintOut = [](std::shared_ptr<Figure> figure) {
        figure->Print();
    };

    function WriteInFile = [](std::shared_ptr<Figure> figure) {
        figure->Write(file);
    };

    std::vector<function> functions({PrintOut, WriteInFile});
    bool awake = true;
    while (awake) {
        if (!message_queue.Empty()) {
            std::vector<std::shared_ptr<Figure>> message = message_queue.Front();
            if (message.empty()) {
                awake = false;
                break;
            }
            file = fopen(FILE_NAME.c_str(), "wb");
            while (!message.empty()) {
                std::shared_ptr<Figure> figure_pointer = message.back();
                message.pop_back();
                for (auto function: functions) {
                    function(figure_pointer);
                }
            }
            fclose(file);
            message_queue.Pop();
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "No enough parameters!\n";
        return -1;
    }
    size_t buffer_size;
    try {
        buffer_size = std::stoi(std::string(argv[1]));
    }
    catch (std::exception &exeption) {
        std::cout << exeption.what() << "\n";
        return -1;
    }
    std::srand(time(NULL));
    std::thread thread(ThreadFunction);
    std::vector<std::shared_ptr<Figure>> figures;
    unsigned short figure_type;
    std::cout << "Number of figure type (1 - Rectangle; 2 - Trapeze; 3 - Rhombus)\n";
    while (std::cin >> figure_type) {
        if (figure_type == RECTANGLE_ID_TYPE) {
            figures.push_back(Factory<int, Rectangle<int>>::CreateFigure());
        } else if (figure_type == TRAPEZE_ID_TYPE) {
            figures.push_back(Factory<int, Trapeze<int>>::CreateFigure());
        } else if (figure_type == RHOMBUS_ID_TYPE) {
            figures.push_back(Factory<int, Rhombus<int>>::CreateFigure());
        }
        if (figures.size() == buffer_size) {
            message_queue.Push(figures);
            bool thread_work_done = false;
            while (!thread_work_done) {
                if (message_queue.Empty()) {
                    thread_work_done = true;
                }
            }
            figures.clear();
        }
    }
    message_queue.Push(figures);
    figures.clear();
    thread.join();
}