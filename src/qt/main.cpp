#include <thread>
#include "main.h"
#include "cpu.h"

void threadfunc(CPU *c)
{

}

int main(int argc, char ** argv)
{	
	QApplication app(argc, argv);

	CPU *c = new CPU();
	std::thread t(threadfunc, c);

	MainWindow w(c->screen);
	w.show();
	return app.exec();
}
