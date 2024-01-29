#include "technicianview.h"
#include "ui_technicianview.h"


TechnicianView::TechnicianView(ConfigHelper *config, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TechnicianView)
{
    ui->setupUi(this);

    // Store config class ptr
    this->config = config;

    // Set focus policy so we can receive keyboard events
    this->setFocusPolicy(Qt::StrongFocus);

    // Populate fields with data from config
    populate();

    // Start run loop for OSC and button polling
     loopTimer = new QTimer(this);
     connect(loopTimer, &QTimer::timeout, this, &TechnicianView::loop);
     loopTimer->start(0);  // As fast as possible :)
}

void TechnicianView::populate() {
    // Create the info tempalte
    QString info;
    info.append(
        "<style>"
        "   * { "
        "       text-align: center;"
        "       color: white;"
        "       font-weight: normal;"
        "       font-size: 10px;"
        "       line-height: 2px;"
        "   }"
        ""
        "   b {"
        "       font-weight: bold;"
        "   }"
        "</style>"

        "<h3>OSC</h3>"
        "<p><b>HOST:</b> {{osc_host}} <b>HOST PORT</b>: {{osc_host_port}} <b>PORT</b>: {{osc_port}}"
        "<p><b>BUTTON H:</b> {{button_h_path}}"
        "<p><b>BUTTON C:</b> {{button_c_path}}"
        "<p><b>BUTTON B:</b> {{button_b_path}}"

        "<h3>NDI</h3>"
        "<p><b>STREAM:</b> {{ndi_stream}}"
        );


    // Step index and string for template engine
    size_t index = 0;
    std::string cinfo = info.toStdString();

    // Iterate through elements and replace
    while (index != std::variant_npos) {
        index = 0;

        // Find open tag
        index = cinfo.find("{{", index);
        if (index == std::variant_npos) continue;
        size_t first = index;   // Increment index so we strip the tag

        // Find close tag
        index = cinfo.find("}}", index);
        if (index == std::variant_npos) continue;
        size_t last = index + 2;        // Dont increment index so we strip the tag

        // Get element value
        std::string name = cinfo.substr(first+2, (last-2)-(first+2));

        // Find element value and replace with value from config
        if (strcmp(name.c_str(), "osc_host") == 0) {
            std::string val = "";

            // Check value exists and is a string
            if (config->getParameter("osc_host").isString()) {
                val = config->getParameter("osc_host").toString().toStdString();
            }

            cinfo.replace(first, last-first, val);
            continue;
        }

        if (strcmp(name.c_str(), "osc_host_port") == 0) {
            std::string val = "";

            // Check value exists and is a double
            if (config->getParameter("osc_host_port").isDouble()) {
                int num_val = config->getParameter("osc_host_port").toInt();

                // Convert integer to string
                char *val_ptr;
                val_ptr = (char *) malloc(sizeof(char) * (int) log10(num_val));

                sprintf(val_ptr, "%d", num_val);
                val = val_ptr;

                // Make sure we free the allocated memory from before, no mem leaks here ;)
                free(val_ptr);
            }

            cinfo.replace(first, last-first, val);
            continue;
        }

        if (strcmp(name.c_str(), "osc_port") == 0) {
            std::string val = "";

            // Check value exists and is a double
            if (config->getParameter("osc_port").isDouble()) {
                int num_val = config->getParameter("osc_port").toInt();

                // Convert integer to string
                char *val_ptr;
                val_ptr = (char *) malloc(sizeof(char) * (int) log10(num_val));

                sprintf(val_ptr, "%d", num_val);
                val = val_ptr;

                // Make sure we free the allocated memory from before, no mem leaks here ;)
                free(val_ptr);
            }

            cinfo.replace(first, last-first, val);
            continue;
        }

        if (strcmp(name.c_str(), "button_h_path") == 0) {
            std::string val = "";

            // Check value exists and is a string
            if (config->getParameter("button_h_path").isString()) {
                val = config->getParameter("button_h_path").toString().toStdString();
            }

            cinfo.replace(first, last-first, val);
            continue;
        }

        if (strcmp(name.c_str(), "button_c_path") == 0) {
            std::string val = "";

            // Check value exists and is a string
            if (config->getParameter("button_c_path").isString()) {
                val = config->getParameter("button_c_path").toString().toStdString();
            }

            cinfo.replace(first, last-first, val);
            continue;
        }

        if (strcmp(name.c_str(), "button_b_path") == 0) {
            std::string val = "";

            // Check value exists and is a string
            if (config->getParameter("button_b_path").isString()) {
                val = config->getParameter("button_b_path").toString().toStdString();
            }

            cinfo.replace(first, last-first, val);
            continue;
        }

        if (strcmp(name.c_str(), "ndi_stream") == 0) {
            std::string val = "";

            // Check value exists and is a string
            if (config->getParameter("ndi_stream").isString()) {
                val = config->getParameter("ndi_stream").toString().toStdString();
            }

            cinfo.replace(first, last-first, val);
            continue;
        }
    }


    std::string console_id = "";

    // Check value exists and is a double
    if (config->getParameter("console_id").isDouble()) {
        int num_val = config->getParameter("console_id").toInt();

        // Convert integer to string
        char *val_ptr;
        val_ptr = (char *) malloc(sizeof(char) * (int) log10(num_val));

        sprintf(val_ptr, "%d", num_val);
        console_id = val_ptr;

        // Make sure we free the allocated memory from before, no mem leaks here ;)
        free(val_ptr);
    }

    ui->consoleIdentifierLabel->setText(console_id.c_str());
    ui->infoTextEdit->setText(QString::fromStdString(cinfo));
}


// Input methods
 void TechnicianView::loop() {
    // This is a run loop for polling button presses and triggering OSC
    static uint32_t hLastTick = 0;
    static uint32_t cLastTick = 0;
    static uint32_t bLastTick = 0;

    // This is a bit of a hack, make sure we dont exit out straight away (ignore first button press)
    static int hState = 0;
    static int cState = 0;
    static int bState = 0;



    int hNewState = gpioRead(17);
    int cNewState = gpioRead(22);
    int bNewState = gpioRead(27);

    uint32_t tick = gpioTick();

    // Handle H Button
    if (hNewState != hState) {
        if (tick > hLastTick + 200000) { // uS
            hLastTick = tick;

            hState = hNewState;
            if (hState == false)
                handleButtonHPressed();
        }
    }

    // Handle C Button
    if (cNewState != cState) {
        if (tick > cLastTick + 200000) { // uS
            cLastTick = tick;

            cState = cNewState;
            if (cState == false)
                handleButtonCPressed();
        }
    }

    // Handle B Button
    if (bNewState != bState) {
        if (tick > bLastTick + 200000) { // uS
            bLastTick = tick;

            bState = bNewState;
            if (bState == false)
                handleButtonBPressed();
        }
    }
 }

void TechnicianView::keyPressEvent(QKeyEvent *event) {
    switch (event->nativeScanCode()) {
        case 331: {
            // Button H / Left
            handleButtonHPressed();
            break;
        }

        case 336: {
            // Button C / Down
            handleButtonCPressed();
            break;
        }

        case 333: {
            // Button B / Right
            handleButtonBPressed();
            break;
        }
    }
}

void TechnicianView::handleButtonHPressed() {
    int val = 0;

    if (config->getParameter("console_id").isDouble()) {
        val = config->getParameter("console_id").toInt();
    }

    // Clamp values
    val--;
    if (val < 1) val = 20;

    // Update config and repopulate
    config->setParameter("console_id", QJsonValue(val));
    populate();
}

void TechnicianView::handleButtonCPressed() {
    // Save config to file and close technician view
    config->saveConfig();
    emit transitionToMainView();
}

void TechnicianView::handleButtonBPressed() {
    int val = 0;

    if (config->getParameter("console_id").isDouble()) {
        val = config->getParameter("console_id").toInt();
    }

    // Clamp values
    val++;
    if (val > 20) val = 1;

    // Update config and repopulate
    config->setParameter("console_id", QJsonValue(val));
    populate();
}

TechnicianView::~TechnicianView()
{
    delete ui;

    loopTimer->stop();
    delete loopTimer;
}
