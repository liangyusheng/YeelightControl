from Modules import *


def main():
    app = QApplication([])
    app.setApplicationName("Yeelight Control")
    app.setApplicationVersion("1.0")
    
    w = MainWindow()
    w.setWindowTitle("Yeelight Control")
    w.resize(370, 470)
    w.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
