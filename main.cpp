#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string>
#include <iostream>

#include <GLFW/glfw3.h>


#include "src/Experiment.h"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Система управления оптовым складом", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImFont* font = io.Fonts->AddFontFromFileTTF("./CalibriRegular.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());    

    // Our state
    bool show_demo_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    Experiment exp("products.txt");

    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!exp.varsDefined) {
            ImGui::OpenPopup("Конфигурация");
            if (ImGui::BeginPopupModal("Конфигурация", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                static char Nbuf[64] = "16";
                static char Kbuf[64] = "12";
                static char Mbuf[64] = "5";
                static char minDeliveryTimeBuf[64] = "1";
                static char maxDeliveryTimeBuf[64] = "5";
                static char maxRequestQntBuf[64] = "50";
                static char minProductQntBuf[64] = "3";
                static char replenishmentSizeBuf[64] = "10";

                ImGui::Text("Период моделирования:");
                ImGui::SameLine(270); 
                
                ImGui::InputText("N", Nbuf, 64, ImGuiInputTextFlags_CharsDecimal);

                ImGui::Text("Кол-во видов продуктовых товаров:");
                ImGui::SameLine(270); 
                ImGui::InputText("K", Kbuf, 64, ImGuiInputTextFlags_CharsDecimal);

                ImGui::Text("Кол-во торговых точек:");
                ImGui::SameLine(270); 
                ImGui::InputText("M", Mbuf, 64, ImGuiInputTextFlags_CharsDecimal);
                
                ImGui::Text("Максимальный размер заказа позиции:");
                ImGui::SameLine(270); 
                ImGui::InputText("штук", maxRequestQntBuf, 64, ImGuiInputTextFlags_CharsDecimal);

                ImGui::Text("Минимальный порог кол-ва упаковок:");
                ImGui::SameLine(270); 
                ImGui::InputText("уп.", minProductQntBuf, 64, ImGuiInputTextFlags_CharsDecimal);

                ImGui::Text("Количество упаковок на докупку:");
                ImGui::SameLine(270); 
                ImGui::InputText("уп.##1", replenishmentSizeBuf, 64, ImGuiInputTextFlags_CharsDecimal);

                ImGui::Text("Диапазон времени на поставку:");
                ImGui::SameLine(270); 
                ImGui::InputText("от", minDeliveryTimeBuf, 64, ImGuiInputTextFlags_CharsDecimal);
                ImGui::Text(" ");
                ImGui::SameLine(270);
                ImGui::InputText("до", maxDeliveryTimeBuf, 64, ImGuiInputTextFlags_CharsDecimal);

                ImGui::Separator();
                if (ImGui::Button("Начать", ImVec2(120, 0))) { 
                    if (strlen(Nbuf) && strlen(Mbuf) && strlen(Kbuf)
                        && strlen(minDeliveryTimeBuf) && strlen(maxDeliveryTimeBuf))
                    {   
                        int N, K, M;
                        int minDeliveryTime, maxDeliveryTime;
                        int maxRequestQnt, minProductQnt;
                        int replenishmentSize;

                        K = atoi(Kbuf);
                        N = atoi(Nbuf);
                        M = atoi(Mbuf);
                        minDeliveryTime = atoi(minDeliveryTimeBuf);
                        maxDeliveryTime = atoi(maxDeliveryTimeBuf);
                        maxRequestQnt = atoi(maxRequestQntBuf);
                        minProductQnt = atoi(minProductQntBuf);
                        replenishmentSize = atoi(replenishmentSizeBuf);

                        if (K < 12 || K > 20 || N < 10 || N > 30 || M < 3 || M > 9 
                            || minDeliveryTime < 1 || maxDeliveryTime > 5 || maxRequestQnt <= 0
                            || minProductQnt <= 0 || replenishmentSize <= 0) 
                        {
                            std::cout << "Invalid input" << std::endl;
                        } else {
                            exp.daysNumber = N;
                            exp.productsNumber = K;
                            exp.shopsNumber = M;
                            exp.maxRequestQuantity = maxRequestQnt;
                            exp.minPackagesNumber = minProductQnt;
                            exp.minDeliveryTime = minDeliveryTime;
                            exp.maxDeliveryTime = maxDeliveryTime;
                            exp.replenishmentSize = replenishmentSize;

                            exp.warehouse.minPackages = exp.minPackagesNumber;

                            exp.varsDefined = true;
                            exp.nextDay();
                            std::cout << "Saved params" << std::endl;
                        }
                    } else {
                        std::cout << "Error" << std::endl;
                        ImGui::OpenPopup("Error");
                        bool unused_open = true;
                        if (ImGui::BeginPopupModal("Error", &unused_open)) {
                            ImGui::Text("Hello from Stacked The Second!");
                            if (ImGui::Button("Close"))
                                ImGui::CloseCurrentPopup();
                            ImGui::EndPopup();
                        }
                    }
                    
                    if (exp.varsDefined) {
                        ImGui::CloseCurrentPopup(); 
                    }
                }
                ImGui::EndPopup();
            }
              
        }
        if (show_demo_window && exp.varsDefined)
            ImGui::ShowDemoWindow(&show_demo_window);


        if (exp.varsDefined) {
            ImGui::Begin("Склад");
            font->AddRemapChar(0xC1, 0x0411);
            static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
            if (ImGui::BeginTable("split", 7, flags)) {
                ImGui::TableSetupColumn("Товар");
                ImGui::TableSetupColumn("Количество");
                ImGui::TableSetupColumn("Количество в упаковке");
                ImGui::TableSetupColumn("Срок годности");
                ImGui::TableSetupColumn("Цена");
                ImGui::TableSetupColumn("Цена после уценки");
                ImGui::TableSetupColumn("Уценка");
                ImGui::TableHeadersRow();

                int i = 0;
                for (auto& p : exp.warehouse.storage) {
                    ImGui::TableNextRow();

                    if (ImGui::TableSetColumnIndex(0))
                        ImGui::Text(p.first.c_str());

                    if (ImGui::TableSetColumnIndex(1))
                        ImGui::Text(std::to_string(p.second.second).c_str());

                    if (ImGui::TableSetColumnIndex(2))
                        ImGui::Text(std::to_string(p.second.first.quantity).c_str());

                    if (ImGui::TableSetColumnIndex(3))
                        ImGui::Text(std::to_string(p.second.first.product.expDate).c_str());

                    if (ImGui::TableSetColumnIndex(4))
                        ImGui::Text(std::to_string(p.second.first.product.price).c_str());

                    if (ImGui::TableSetColumnIndex(5))
                        ImGui::Text(std::to_string(p.second.first.product.price - p.second.first.product.price * (p.second.first.discountPercent / 100.0)).c_str()); 

                    if (ImGui::TableSetColumnIndex(6)) {
                        ImGui::Text("%d %%", p.second.first.discountPercent);
                        ImGui::SameLine();
                        if (ImGui::SmallButton(("-##" + std::to_string(i)).c_str())) {
                            p.second.first.decreaseDiscount();
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton(("+##" + std::to_string(i)).c_str())) {
                            p.second.first.increaseDiscount();
                        }
                    }
                    ++i;
                }
                ImGui::EndTable();
                
            }
            ImGui::End();
        }

        if (exp.varsDefined) {
            ImGui::Begin("Заказы с продуктовых точек");
            for (int i = 0; i < exp.warehouse.completedOrders.orders.size(); ++i) {
                ImGui::Text("Заказ %d", i + 1);
                if (ImGui::BeginTable("order", 4)) {
                    ImGui::TableSetupColumn("Наименование");
                    ImGui::TableSetupColumn("Количество");
                    ImGui::TableSetupColumn("Отпущено упаковок");
                    ImGui::TableSetupColumn("Общая сумма");
                    ImGui::TableHeadersRow();

                    for (const auto& order : exp.warehouse.completedOrders.orders[i].orders) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text(order.name.c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", order.requestedQuantity);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", order.soldPackagesQuantity);

                        ImGui::TableSetColumnIndex(3);       
                        ImGui::Text("%f", order.price);                 
                    }

                    ImGui::EndTable();
                }
                ImGui::Text("\n\n");
            }
            ImGui::End();
        }

        if (exp.varsDefined) {
            ImGui::Begin("Поставки");
            if (ImGui::BeginTable("delivery", 3)) {
                ImGui::TableSetupColumn("Наименование");
                ImGui::TableSetupColumn("Количество упаковок");
                ImGui::TableSetupColumn("Дата доставки");
                ImGui::TableHeadersRow();

                for (const auto& order : exp.warehouse.replenishment.products) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text(order.name.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", order.quantity);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", order.deliveryDate);
                }

                ImGui::EndTable();
            }
            ImGui::End();
        }
        if (exp.varsDefined) {
            ImGui::Begin("Управление");
            ImGui::Text("День: %d из %d\n\n", exp.currentDay, exp.daysNumber);  

            ImGui::Text("Прибыль: %f", exp.profit);
            ImGui::Text("Убыток:    %f\n\n", exp.loss);

            if (ImGui::Button("Сгенерировать заказы"))
                exp.generateOrders();

            ImGui::Text(" ");

            if (ImGui::Button("Следующий день"))
                exp.nextDay();
            ImGui::SameLine();
            if (ImGui::Button("До конца"))                       
                exp.skipDays();

            ImGui::Text(" ");
            
            if (ImGui::Button("Выход"))                       
                ImGui::OpenPopup("Вы хотите выйти?");

            if (ImGui::BeginPopupModal("Вы хотите выйти?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Вы действительно хотите выйти?\nВесь процесс будет утерян\n\n");
                ImGui::Separator();
                if (ImGui::Button("Выйти", ImVec2(120, 0))) { 
                    ImGui::CloseCurrentPopup(); 
                    break;
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Отмена", ImVec2(120, 0))) { 
                    ImGui::CloseCurrentPopup(); 
                }
                ImGui::EndPopup();
            }

            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}