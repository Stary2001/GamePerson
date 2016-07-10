#include <QtWidgets>
#include "render.h"
#include "screen.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(GBScreen *s);

private:
	RenderWidget *win;
};