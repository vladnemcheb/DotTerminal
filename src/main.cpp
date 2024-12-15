#include <QApplication>
#include <QMainWindow>
#include <QDesktopServices> // добавляем новую библиотеку
#include <QKeySequence>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QMenu>
#include <QActionEvent>
#include "qtermwidget.h"
#include <filesystem>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <QPainter>

void activateLink(const QUrl &url, bool fromContextMenu) {
  /* Функция, которая ничего не возвращает, принимает на вход URL и 
  fromContextMenu.
  Открывает ссылку в доступном браузере */
  if (QApplication::keyboardModifiers() & Qt::ControlModifier ||
        fromContextMenu) {
    QDesktopServices::openUrl(url);
  }
}

int main(int argc, char *argv[]) {
  if (!std::filesystem::exists("currentTheme.txt")) {
        std::ofstream out;          // поток для записи
        out.open("currentTheme.txt");      // открываем файл для записи
        if (out.is_open())
        {
            out << "Tango" << std::endl;
        }
        out.close();
  }
  if (argv[1] != nullptr) {
    std::ofstream out;          // поток для записи
    out.open("currentTheme.txt");      // открываем файл для записи
    if (out.is_open())
    {
        out << argv[1] << std::endl;
    }
    out.close();
  }
  else {

  }
  // Создаем окно и приложение
  QApplication app(argc, argv);
  QMainWindow *mainWindow = new QMainWindow();
  mainWindow->setProperty("windowOpacity", 0.85);
  mainWindow->setGeometry(20,20,700,500);

  // Создаем объект консоли
  QTermWidget *console = new QTermWidget();
  // Добавляем шрифт терминала
  QFont font = QApplication::font();
  font.setFamily("Monospace"); // задаем шрифт monospace
  font.setPointSize(14); // задаем размер шрифта в pt

  console->setTerminalFont(font); // задаем наши характеристики шрифта в консоль
  console->setBlinkingCursor(true); // мерцающий курсор
  // Добавляем внутренние отступы 10 пикселей
  console->setMargin(10);

  if (std::filesystem::exists("currentTheme.txt")) {
    std::string line;

    std::ifstream in("currentTheme.txt"); // окрываем файл для чтения
    if (in.is_open())
    {
      while (std::getline(in, line))
      {
        QString qstr = QString::fromStdString(line);
        console->setColorScheme(qstr);
      }
    }
    in.close();     // закрываем файл
  }
  else {
      console->setColorScheme(argv[1]);
  }

  // Подключаем
  QObject::connect(console, &QTermWidget::termKeyPressed, mainWindow,
					[=](const QKeyEvent *key) -> void { // проверяем нажатия клавиш
						if (key->matches(QKeySequence::Copy)) {
							console->copyClipboard(); // ctrl+c
						}
					});
  // Подключаем функцию активации ссылки
  QObject::connect(console, &QTermWidget::urlActivated, mainWindow,
					activateLink);
  
  // Показ окна
  QObject::connect(console, SIGNAL(finished()), mainWindow, SLOT(close()));
  mainWindow->setCentralWidget(console);
  mainWindow->show();

  return app.exec();
}
