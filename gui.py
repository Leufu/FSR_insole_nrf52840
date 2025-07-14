import csv
import sys
import serial
import serial.tools.list_ports
import numpy as np
import pyqtgraph as pg
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QWidget,
    QPushButton, QComboBox, QLineEdit, QLabel
)
from PyQt5.QtCore import QTimer
from PyQt5.QtGui import QPixmap

class RealTimeFSRPlot(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("FSR BLE Viewer")
        self.resize(800, 600)

        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)
        self.layout = QVBoxLayout(self.central_widget)

        self.plot_widget = pg.PlotWidget()
        self.layout.addWidget(self.plot_widget)

        self.serial_port_combo = QComboBox()
        self.layout.addWidget(self.serial_port_combo)

        self.controls_layout = QHBoxLayout()
        self.find_ports_button = QPushButton("Buscar puertos")
        self.find_ports_button.clicked.connect(self.find_serial_ports)
        self.controls_layout.addWidget(self.find_ports_button)

        self.connect_button = QPushButton("Conectar")
        self.connect_button.clicked.connect(self.connect_to_serial)
        self.controls_layout.addWidget(self.connect_button)

        self.record_button = QPushButton("Grabar")
        self.record_button.clicked.connect(self.toggle_recording)
        self.controls_layout.addWidget(self.record_button)

        self.filename_input = QLineEdit()
        self.filename_input.setPlaceholderText("Nombre del archivo CSV")
        self.controls_layout.addWidget(self.filename_input)

        self.duration_input = QLineEdit()
        self.duration_input.setPlaceholderText("Duración (seg)")
        self.controls_layout.addWidget(self.duration_input)

        self.layout.addLayout(self.controls_layout)

        self.timer_label = QLabel("Tiempo: 0 s")
        self.layout.addWidget(self.timer_label)

        self.status_label = QLabel("Estado: Sin grabar")
        self.layout.addWidget(self.status_label)

        self.image_label = QLabel()
        self.pixmap_default = QPixmap(100, 100)
        self.pixmap_default.fill()
        self.image_label.setPixmap(self.pixmap_default)
        self.layout.addWidget(self.image_label)

        self.data_buffer = np.zeros(1000)
        self.plot_data = self.plot_widget.plot(self.data_buffer, pen='r')
        self.plot_widget.setYRange(0, 1024, padding=0)
        self.plot_widget.setLabel('left', 'FSR0')
        self.plot_widget.setLabel('bottom', 'Muestras')

        self.serial = None
        self.recording = False
        self.csv_data = []
        self.timer_value = 0
        self.target_duration = 0

        self.update_timer_obj = QTimer()
        self.update_timer_obj.timeout.connect(self.update_plot)
        self.update_timer_obj.start(20)

        self.countdown_timer = QTimer()
        self.countdown_timer.timeout.connect(self.update_timer)

    def find_serial_ports(self):
        self.serial_port_combo.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.serial_port_combo.addItem(port.device)

    def connect_to_serial(self):
        port = self.serial_port_combo.currentText()
        try:
            if self.serial:
                self.serial.close()
            self.serial = serial.Serial(port, 115200)
            self.status_label.setText(f"Conectado a {port}")
        except Exception as e:
            self.status_label.setText(f"Error: {e}")

    def toggle_recording(self):
        if not self.serial or not self.serial.is_open:
            self.status_label.setText("No conectado")
            return

        try:
            self.target_duration = int(self.duration_input.text())
        except ValueError:
            self.target_duration = 0

        self.timer_value = 0
        self.csv_data = []
        self.recording = True
        self.countdown_timer.start(1000)
        self.status_label.setText("Grabando...")
        self.record_button.setEnabled(False)

    def update_timer(self):
        self.timer_value += 1
        self.timer_label.setText(f"Tiempo: {self.timer_value} s")
        if self.timer_value >= self.target_duration:
            self.countdown_timer.stop()
            self.recording = False
            self.save_csv()
            self.status_label.setText("Grabación finalizada")
            self.record_button.setEnabled(True)

    def save_csv(self):
        filename = self.filename_input.text()
        if not filename.endswith(".csv"):
            filename += ".csv"
        try:
            with open(filename, mode='w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(["timestamp", "FSR0", "FSR1", "FSR2", "FSR3"])
                writer.writerows(self.csv_data)
            self.status_label.setText(f"Guardado en {filename}")
        except Exception as e:
            self.status_label.setText(f"Error al guardar: {e}")

    def update_plot(self):
        if self.serial and self.serial.in_waiting:
            try:
                line = self.serial.readline().decode('utf-8').strip()
                parts = line.split(',')
                if len(parts) == 5:
                    ts, fsr0, fsr1, fsr2, fsr3 = map(float, parts)
                    self.data_buffer = np.roll(self.data_buffer, -1)
                    self.data_buffer[-1] = fsr0
                    self.plot_data.setData(self.data_buffer)

                    if self.recording:
                        self.csv_data.append([ts, fsr0, fsr1, fsr2, fsr3])
            except Exception as e:
                print("Error:", e)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = RealTimeFSRPlot()
    window.show()
    sys.exit(app.exec_())

