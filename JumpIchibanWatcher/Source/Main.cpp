#include "App.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    App logic(app);

    g_App = &logic;

    g_App->createProduct();
    g_App->showProductInfo();

    return app.exec();
}