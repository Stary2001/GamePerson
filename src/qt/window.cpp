#include "main.h"
#include "render.h"

MainWindow::MainWindow(GBScreen *s)
{
	win = new RenderWidget(this, s);
	setCentralWidget(win);

	QMenu *emuMenu = menuBar()->addMenu(tr("&Emulation"));
	QAction *startAct = new QAction(tr("&Start"), this);
	QAction *stopAct = new QAction(tr("&Stop"), this);
	emuMenu->addAction(startAct);
	emuMenu->addAction(stopAct);

	statusBar()->showMessage(tr("Ready"));

	setUnifiedTitleAndToolBarOnMac(true);
}