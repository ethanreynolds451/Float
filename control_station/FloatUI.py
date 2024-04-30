''' UI Layout

        Header

        Serial Port             Serial Baudrate            Address             Disconnected
        _______             ____________          _______
        _______             ____________          _______                 Connect



                                                            Buttons

        

                      Serial MonitorConsole                                            Console
      |                                                                          |       |                                                |
      |                                                                          |       |                                                |
      |                                                                          |       |                                                |
      |                                                                          |       |                                                |
      |                                                                          |       |                                                |

'''

import os
import tkinter as tk
from tkinter import *
import serial
import time
from datetime import datetime
import serial.tools.list_ports
import re
from tkinter import filedialog
from pathlib import Path
from tkinter import ttk
from tkinter import messagebox

class FloatUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("MATE Float")
        self.geometry('800x600')

        self.send_status = tk.StringVar(value="No Message Sent")
        self.active_input = tk.StringVar(value="Enter Commands Here")
        self.active = ""
        # Local
        self.transmission_request = "RQTX="
        self.data_header = "DATA:"
        # Basic Control
        self.confirm = "CONFRM"
        self.response = "GTRESP"
        self.broadcast = "BRDCST"
        self.reset = "RESET"
        self.empty = "CMEMTY"
        self.fill = "CMFILL"
        self.stop = "CMSTOP"
        # Sensor Retrieval
        self.get_time = "GTTIME"
        self.get_pressure = "GTPRSS"
        # Data Transmission
        self.get_data_count = "GTDTCT"
        self.get_data = "GTDATA"
        self.get_sample_data = "GTSDTA"
        # Compound functions
        self.vertical_profile = "CMVPRO"
        self.down_up = "CMDNUP"
        self.center = "CMCNTR"

        self.directory = tk.StringVar(value=Path.home() / "Desktop")
        self.retrieve = tk.StringVar(value="Retrieve Data")
        self.log = None
        self.recording = False
        self.count = 0

        self.ser = serial.Serial()
        self.ser.baudrate = 115200
        self.ser.write_timeout = 0
        self.port_options = list()
        self.selected_port = tk.StringVar(value="No Port Selected")
        self.baud_options = [110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000,
                        256000]
        self.selected_baud = tk.StringVar(value=self.ser.baudrate)
        self.selected_address = tk.StringVar(value=1)
        self.connection_status = tk.StringVar(value="Disconnected")

        self.create_widgets()

    # <editor-fold desc="Communication">

    def select_directory(self):
        self.directory.set(filedialog.askdirectory(initialdir=self.directory.get()))
        self.savePath['state'] = 'normal'
        self.savePath.delete("1.0", "end")
        self.savePath.insert(tk.END, self.directory.get())
        self.savePath['state'] = 'disabled'

    def data_get(self):
        ## Check to see if the float is ready to relay message
        if self.transmission_request in active_message:
            ## Create a CSV file in selected directory to save data
            try:
                # Create folder path with date if it does not already exist
                main_directory = self.directory.get()
                name = str(datetime.now().date()) + "/"
                folder = main_directory + "/Float_Data/" + name
                # Creat container folder if it does not already exist
                if not os.path.exists(main_directory + "/Float_Data/"):
                    os.mkdir(main_directory + "/Float_Data/")
                # Creat date folder if it does not already exist
                if not os.path.exists(folder):
                    os.mkdir(folder)
                # Create CSV file with time in date directory
                self.log = folder + "/" + str(datetime.utcnow().strftime("%H:%M:%S")).replace(':', '') + ".csv"
                # Write header to file
                with open(self.log, 'w') as file:
                    file.write("Index,Company Number,Time,Depth,Pressure")
                    self.console_println("Created log file " + name + " at " + folder)
                self.console_println("Reading data from float...")
                self.retrieve_data.set("Retrieving ..")
            except:
                self.console_println("Error creating file\nCheck folder permissions")
            try:
                self.start_recording(int(active_message[
                                         str.find(self.transmisson_request) + len(
                                             self.transmisson_request) + 1:len(
                                             active_message) - len(self.transmisson_request)]))
            except:
                self.retrieve_data.set("Retrieve Data")
                self.console_println("Unable to translate data count")
        else:
            self.console_println("No data to read")

    def start_recording(self, n):
        self.recording = True
        self.count = n
        self.recorded = []

    def record(self, message):
            # DATA-1:RA01;UTC=HH:MM:SS;kpa=
            try:
                with open(self.log, 'w') as file:
                    self.recorded.append(0)
                    # Add string of data to file
                    file.write(message)
            except:
                self.console_println("Error retrieving data from float")

    def get_ports(self):
        self.port_options.clear()
        self.port_options.append("No Port Selected")
        p = [tuple(p) for p in list(serial.tools.list_ports.comports())]
        for i in p:
            for x in i:
                if x != "n/a":
                    # if "dev" in x: # THIS DOESNT WORK ON WINDOWS
                    # Needed for serial on Mac - renames port so it can connect, but only if .cu is found
                    x = re.sub(".cu.", "/tty.", x)
                    self.port_options.append(x)

    def disconnect(self):
        self.ser.close()
        self.connection_status.set("Disconnected")

    def connect(self):
        # First, close the port if it is already open
        try:
            self.ser.close()
        except:
            pass
        # Then, attempt to connect to device with selected parameters
        try:
            self.ser.port = self.selected_port.get()
            self.ser.baudrate = self.selected_baud.get()
            self.ser.open()
            self.connection_status.set("Connected")
            self.console_println("*Device connected")
            self.console_println(" Port: " + str(self.ser.port))
            self.console_println(" Baudrate: " + str(self.ser.baudrate))
        except Exception as e:
            self.disconnect()
            self.console_println("*Error opening serial port:")
            self.console_println(str(e))

    def update_ports(self):
        menu = self.portSelect["menu"]
        menu.delete("0", tk.END)
        self.get_ports()
        for port in self.port_options:
            menu.add_command(label=port,
                             command=lambda value=port: [self.selected_port.set(value), self.disconnect()])
        self.after(3000, self.update_ports)

    def send(self, data):
        try:
            self.ser.write("AT+SEND=".encode())
            self.ser.write(self.selected_address.get().encode())
            self.ser.write(",".encode())
            self.ser.write(str(len(data)).encode())
            self.ser.write(",".encode())
            self.ser.write(data.encode())
            self.ser.write(bytes([0x0D, 0x0A]))
        except:
            if not self.ser.isOpen():
                self.console_println("Serial device disconnected")
            else:
                self.console_println("Error writing to port")
            self.disconnect()

    def data_send(self):
        self.send(data = self.active_input.get())

    def read(self):
        if self.ser.isOpen():
            try:
                if self.ser.inWaiting() > 0:
                    serialData = self.ser.readline().decode().rstrip()
                    self.serialMonitor_println(serialData)
                    self.process_message(serialData)
            except OSError or serial.SerialException:
                self.console_println("Serial device disconnected")
                self.disconnect()
            except Exception as e:
                self.console_println("Read error: {}".format(e))
        self.after(100, self.read)

    def parse_message(self, data):
        global active_message
        try:
            start = data.find(",", data.find(",") + 1)
            end = data.rfind(",", 0, data.rfind(","))
            message = data[start + 1: end]
            active_message = message
            return message
        except:
            self.console_println("Invalid Data")

    def process_message(self, data):
        if "+OK" in data:
            self.console_println("Send sucessful")
        if "+RCV" in data:
            message = self.parse_message(data)
            self.console_println(message)
            if self.recording:
                self.record(message)
    # </editor-fold>

    # <editor-fold desc="UI Layout">
    # Tkinter widget layout

    class Layout:
        # <editor-fold desc="Header">
        maxWidth = 8
        maxHeight = 6
        headerRow = 0
        labelRow = 1
        selectorRow = 2
        portColumn = 0
        portColumnSpan = 4
        baudColumn = portColumnSpan
        baudColumnSpan = 1
        addressColumn = portColumnSpan + baudColumnSpan
        addressColumnSpan = 1
        connectionColumn = portColumnSpan + baudColumnSpan + addressColumnSpan
        connectionColumnSpan = 1
        # </editor-fold>

        # <editor-fold desc="Control panel">
        controlPanelRow = 3
        controlPanelRowSpan = 1

        verticalProfileRow = 0
        verticalProfileColumn = 0
        verticalProfileRowSpan = 2
        verticalProfileColumnSpan = 2

        upButtonRow = 0
        upButtonColumn = verticalProfileColumnSpan
        upButtonRowSpan = 1
        upButtonColumnSpan = 1

        downButtonRow = 0
        downButtonColumn = upButtonColumn + upButtonRowSpan
        downButtonRowSpan = 1
        downButtonColumnSpan = 1

        stopButtonRow = 1
        stopButtonColumn = verticalProfileColumnSpan
        stopButtonRowSpan = 1
        stopButtonColumnSpan = 2
        stopButtonWidth = 8
        stopButtonHeight = 1

        fillEmptyButtonRow = 0
        fillEmptyButtonColumn = stopButtonColumn + stopButtonColumnSpan
        fillEmptyButtonRowSpan = 1
        fillEmptyButtonColumnSpan = 2
        fillEmptyButtonWidth = 6
        fillEmptyButtonHeight = 1

        centerButtonRow = 1
        centerButtonColumn = stopButtonColumn + stopButtonColumnSpan
        centerButtonRowSpan = 1
        centerButtonColumnSpan = 2
        centerButtonWidth = 6
        centerButtonHeight = 1

        savePathRow = 0
        savePathColumn = fillEmptyButtonColumn + fillEmptyButtonColumnSpan
        savePathColumnSpan = 4

        directorySelectRow = savePathRow
        directorySelectColumn = savePathColumn + savePathColumnSpan
        directorySelectColumnSpan = 1

        getDataRow = 1
        getDataColumn = savePathColumn
        getDataRowSpan = 1
        getDataColumnSpan = savePathColumnSpan
        getDataWidth = 15
        getDataHeight = 1

        resetButtonRow = 1
        resetButtonColumn = directorySelectColumn
        resetButtonRowSpan = getDataRowSpan
        resetButtonColumnSpan = directorySelectColumnSpan
        # </editor-fold>

        # <editor-fold desc="Communication Panel">
        communicationPanelRow = controlPanelRow + controlPanelRowSpan
        communicationPanelRowSpan = maxWidth

        commandSendRow = 1
        commandSendRowSpan = 1
        commandSendColumn = 0
        commandSendColumnSpan = int(communicationPanelRowSpan / 2) - 2
        commandSendWidth = 20

        dataMonitorRow = commandSendRow + commandSendRowSpan
        dataMonitorRowSpan = 1
        dataMonitorColumn = commandSendColumn
        dataMonitorColumnSpan = int(communicationPanelRowSpan / 2)
        dataMonitorWidth = commandSendWidth
        dataMonitorHeight = 1

        serialMonitorRow = dataMonitorRow + dataMonitorRowSpan
        serialMonitorRowSpan = 1
        serialMonitorColumn = commandSendColumn
        serialMonitorColumnSpan = dataMonitorColumnSpan
        serialMonitorWidth = commandSendWidth
        serialMonitorHeight = 15

        commandSendButtonRow = commandSendRow
        commandSendButtonRowSpan = 1
        commandSendButtonColumn = commandSendColumn + commandSendColumnSpan
        commandSendButtonColumnSpan = 2
        commandSendButtonWidth = 8

        consoleRow = commandSendRow + commandSendRowSpan
        consoleRowSpan = serialMonitorRowSpan + commandSendButtonRowSpan
        consoleColumn = serialMonitorColumnSpan
        consoleColumnSpan = int(communicationPanelRowSpan / 2)
        consoleWidth = 30
        consoleHeight = 20

        clearButtonRow = commandSendRow
        clearButtonRowSpan = 1
        clearButtonColumn = consoleColumn
        clearButtonColumnSpan = 2
        clearButtonWidth = 8
        # </editor-fold>

        paddings = {'padx': 5, 'pady': 5}

    # Fonts
    class Fonts:
        headerFontSize = 32
        headerFontStyle = "Comic Sans"
        headerFontColor = "white"
        labelFontSize = 18
        labelFontStyle = "Comic Sans"
        labelFontColor = "black"
        selectorFontSize = 14
        selectorFontStyle = "Comic Sans"
        selectorFontColor = "black"

    # Colors
    class Colors:
        headerRowColor = "#2784CF"
        labelRowColor = "yellow"
        selectorRowColor = "orange"
        controlPanelRowColor = "red"
        communicationPanelColor = "#24BAD6"

        @classmethod
        def create_colored_frame(cls, parent, row, span, color):
            frame = tk.Frame(parent, bg=color)
            frame.grid(row=row, column=0, rowspan=span, columnspan=parent.Layout.maxWidth, sticky="nsew")
            return frame

        @classmethod
        def create_borders(cls, ui_instance):
            cls.createHeaderColor = cls.create_colored_frame(ui_instance, ui_instance.Layout.headerRow, 1,
                                                             cls.headerRowColor)
            cls.createLabelColor = cls.create_colored_frame(ui_instance, ui_instance.Layout.labelRow, 1,
                                                            cls.labelRowColor)
            cls.createSelectorColor = cls.create_colored_frame(ui_instance, ui_instance.Layout.selectorRow, 1,
                                                               cls.selectorRowColor)
            cls.createControlPanelColor = cls.create_colored_frame(ui_instance, ui_instance.Layout.controlPanelRow, 1,
                                                                   cls.controlPanelRowColor)
            cls.createCommandSendColor = cls.create_colored_frame(ui_instance, ui_instance.Layout.communicationPanelRow,
                                                                  ui_instance.Layout.communicationPanelRowSpan,
                                                                  cls.communicationPanelColor)
    # </editor-fold>

    def center_text(self, event):
        self.addressEntry.delete(0, tk.END)
        self.addressEntry.insert(0, str.center(self.addressEntry.get(), 3))

    def console_print(self, message):
        self.console['state'] = 'normal'
        self.console.insert(tk.END, message)
        self.console['state'] = 'disabled'
        self.console.yview_pickplace("end")

    def console_println(self, message):
        self.console['state'] = 'normal'
        self.console.insert(tk.END, "\n" + message)
        self.console['state'] = 'disabled'
        self.console.yview_pickplace("end")

    def console_clear(self):
        self.console['state'] = 'normal'
        self.console.delete("1.0", "end")
        self.console['state'] = 'disabled'

    def serialMonitor_print(self, message):
        self.serialMonitor['state'] = 'normal'
        self.serialMonitor.insert(tk.END, message)
        self.serialMonitor['state'] = 'disabled'
        self.serialMonitor.yview_pickplace("end")

    def serialMonitor_println(self, message):
        self.serialMonitor['state'] = 'normal'
        self.serialMonitor.insert(tk.END, "\n" + message)
        self.serialMonitor['state'] = 'disabled'
        self.serialMonitor.yview_pickplace("end")

    def serialMonitor_clear(self):
        self.serialMonitor['state'] = 'normal'
        self.serialMonitor.delete("1.0", "end")
        self.serialMonitor['state'] = 'disabled'

    def com_print(self, message):
        self.communicationMonitor['state'] = 'normal'
        self.communicationMonitor.insert(tk.END, message)
        self.communicationMonitor['state'] = 'disabled'
        self.communicationMonitor.yview_pickplace("end")

    def com_println(self, message):
        self.communicationMonitor['state'] = 'normal'
        self.communicationMonitor.insert(tk.END, "\n" + message)
        self.communicationMonitor['state'] = 'disabled'
        self.communicationMonitor.yview_pickplace("end")

    def create_widgets(self):
        # <editor-fold desc="Tkinter Widgets">
        self.Colors.create_borders(self)

        # <editor-fold desc="Header">
        # Header
        self.headerLabel = tk.Label(self, text="Sound School Ocean Engineering", fg=self.Fonts.headerFontColor,
                               font=(self.Fonts.headerFontStyle, self.Fonts.headerFontSize), justify='center',
                               bg=self.Colors.headerRowColor)
        self.headerLabel.grid(row=self.Layout.headerRow, column=0, columnspan=self.Layout.maxWidth, sticky="ew", **self.Layout.paddings)

        # Port
        self.portLabel = tk.Label(self, text="Serial Port", anchor='center', justify='center', fg=self.Fonts.labelFontColor,
                             font=(self.Fonts.labelFontStyle, self.Fonts.labelFontSize), bg=self.Colors.labelRowColor)
        self.portLabel.grid(row=self.Layout.labelRow, column=self.Layout.portColumn, columnspan=self.Layout.portColumnSpan,  sticky="ew",
                             **self.Layout.paddings)
        self.portSelect = tk.OptionMenu(self, self.selected_port, "No Port Selected")
        self.portSelect.config(width=15)
        self.portSelect.grid(row=self.Layout.selectorRow, column=self.Layout.portColumn, columnspan=self.Layout.portColumnSpan,  sticky="ew",
                             **self.Layout.paddings)

        # Baud
        self.baudLabel = tk.Label(self, text="Baudrate", anchor='center', justify='center', fg=self.Fonts.labelFontColor,
                             font=(self.Fonts.labelFontStyle, self.Fonts.labelFontSize), bg=self.Colors.labelRowColor)
        self.baudLabel.grid(row=self.Layout.labelRow, column=self.Layout.baudColumn, columnspan=self.Layout.baudColumnSpan,  sticky="ew",
                             **self.Layout.paddings)
        self.baudSelect = tk.OptionMenu(self, self.selected_baud, *self.baud_options)
        self.selected_baud.trace("w", self.disconnect)
        self.baudSelect.config(width=6)
        self.baudSelect.grid(row=self.Layout.selectorRow, column=self.Layout.baudColumn, columnspan=self.Layout.baudColumnSpan,  sticky="ew",
                             **self.Layout.paddings)

        # Address
        self.addressLabel = tk.Label(self, text="Address", anchor='center', justify='center', fg=self.Fonts.labelFontColor,
                                font=(self.Fonts.labelFontStyle, self.Fonts.labelFontSize), bg=self.Colors.labelRowColor)
        self.addressLabel.grid(row=self.Layout.labelRow, column=self.Layout.addressColumn, columnspan=self.Layout.addressColumnSpan,
                          sticky="ew", **self.Layout.paddings)
        self.addressEntry = tk.Entry(self, textvariable=self.selected_address, justify="center", width=5)
        self.addressEntry.bind("<FocusOut>", self.center_text)
        self.addressEntry.grid(row=self.Layout.selectorRow, column=self.Layout.addressColumn, columnspan=self.Layout.addressColumnSpan,
                          sticky="ew", **self.Layout.paddings)

        # Connection
        self.connectLabel = tk.Label(self, textvariable=self.connection_status, anchor='center', justify='center',
                                fg=self.Fonts.labelFontColor, font=(self.Fonts.labelFontStyle, self.Fonts.labelFontSize),
                                bg=self.Colors.labelRowColor)
        self.connectLabel.grid(row=self.Layout.labelRow, column=self.Layout.connectionColumn, columnspan=self.Layout.connectionColumnSpan,
                          sticky="ew", **self.Layout.paddings)
        self.connectButton = tk.Button(self, text="Connect", command=self.connect, anchor='center', justify='center',
                                  fg=self.Fonts.selectorFontColor, font=(self.Fonts.selectorFontStyle, self.Fonts.selectorFontSize),
                                  bg=self.Colors.selectorRowColor)
        self.connectButton.grid(row=self.Layout.selectorRow, column=self.Layout.connectionColumn, rowspan=1,
                           columnspan=self.Layout.connectionColumnSpan, sticky="ew", **self.Layout.paddings)
        # </editor-fold>

        # <editor-fold desc="Control Panel">
        self.controlPanel = tk.Frame(self, bg=self.Colors.controlPanelRowColor)
        self.controlPanel.grid(row=self.Layout.controlPanelRow, column=0, rowspan=1, columnspan=self.Layout.maxWidth,  sticky="ew",
                             **self.Layout.paddings)

        # Vertical Profile Command Shortcut
        self.verticalProfile = tk.Button(self.controlPanel, text="Vertical Profile", width=15, height=4,
                                    command=lambda: self.send(self.vertical_profile))
        self.verticalProfile.grid(row=self.Layout.verticalProfileRow, column=self.Layout.verticalProfileColumn,
                             rowspan=self.Layout.verticalProfileRowSpan, columnspan=self.Layout.verticalProfileColumnSpan,
                              sticky="ew",
                             **self.Layout.paddings)

        # Up Button
        self.upButton = tk.Button(self.controlPanel, text="Up", width=4, height=1, command=lambda: self.send(self.empty))
        self.upButton.grid(row=self.Layout.upButtonRow, column=self.Layout.upButtonColumn, rowspan=self.Layout.upButtonRowSpan,
                      columnspan=self.Layout.upButtonColumnSpan, sticky="ew", **self.Layout.paddings)

        # Down Button
        self.downButton = tk.Button(self.controlPanel, text="Down", width=4, height=1, command=lambda: self.send(self.fill))
        self.downButton.grid(row=self.Layout.downButtonRow, column=self.Layout.downButtonColumn, rowspan=self.Layout.downButtonRowSpan,
                        columnspan=self.Layout.downButtonColumnSpan, sticky="ew", **self.Layout.paddings)

        # Stop Button
        self.stopButton = tk.Button(self.controlPanel, text="Stop", width=self.Layout.stopButtonWidth, height=self.Layout.stopButtonHeight,
                               command=lambda: self.send(self.stop))
        self.stopButton.grid(row=self.Layout.stopButtonRow, column=self.Layout.stopButtonColumn, rowspan=self.Layout.stopButtonRowSpan,
                        columnspan=self.Layout.stopButtonColumnSpan, sticky="ew", **self.Layout.paddings)

        # Up and Down (fill then empty)
        self.fillEmptyButton = tk.Button(self.controlPanel, text="Down / Up", width=self.Layout.fillEmptyButtonWidth,
                                    height=self.Layout.fillEmptyButtonHeight, command=lambda: self.send(self.down_up))
        self.fillEmptyButton.grid(row=self.Layout.fillEmptyButtonRow, column=self.Layout.fillEmptyButtonColumn,
                             rowspan=self.Layout.fillEmptyButtonRowSpan, columnspan=self.Layout.fillEmptyButtonColumnSpan,
                             sticky="ew", **self.Layout.paddings)

        # Move to center
        self.centerButton = tk.Button(self.controlPanel, text="Center", width=self.Layout.centerButtonWidth,
                                 height=self.Layout.centerButtonHeight, command=lambda: self.send(self.center))
        self.centerButton.grid(row=self.Layout.centerButtonRow, column=self.Layout.centerButtonColumn,
                          rowspan=self.Layout.centerButtonRowSpan, columnspan=self.Layout.centerButtonColumnSpan,  sticky="ew",
                             **self.Layout.paddings)

        # File Path
        self.savePath = tk.Text(self.controlPanel, width=30, height=1)
        self.savePath.grid(row=self.Layout.savePathRow, column=self.Layout.savePathColumn, rowspan=1,
                      columnspan=self.Layout.savePathColumnSpan,
                      sticky="ew", **self.Layout.paddings)
        self.savePath.insert(tk.END, self.directory.get())
        self.savePath['state'] = 'disabled'

        # File Path Select
        self.directorySelect = tk.Button(self.controlPanel, text="Select", anchor='center', justify='center',
                                    command=lambda: self.select_directory())
        self.directorySelect.grid(row=self.Layout.directorySelectRow, column=self.Layout.directorySelectColumn, rowspan=1,
                             columnspan=self.Layout.directorySelectColumnSpan, sticky="ew", **self.Layout.paddings)

        # Get Data
        self.getData = tk.Button(self.controlPanel, width=self.Layout.getDataWidth, height=self.Layout.getDataHeight,
                            textvariable=self.retrieve,
                            anchor='center', justify='center', command=lambda: self.get())
        self.getData.grid(row=self.Layout.getDataRow, column=self.Layout.getDataColumn, rowspan=self.Layout.getDataRowSpan,
                     columnspan=self.Layout.getDataColumnSpan, sticky="ew", **self.Layout.paddings)

        self.resetButton = tk.Button(self.controlPanel, text="Reset", anchor='center', justify='center',
                                command=lambda: self.send(self.reset))
        self.resetButton.grid(row=self.Layout.resetButtonRow, column=self.Layout.resetButtonColumn, rowspan=1,
                         columnspan=self.Layout.resetButtonColumnSpan, sticky="ew", **self.Layout.paddings)

        self.controlPanel.grid_rowconfigure(0, weight=1)
        self.controlPanel.grid_columnconfigure(0, weight=1)
        # </editor-fold>

        # <editor-fold desc="Communication Panel">
        self.communicationPanel = tk.Frame(self, bg=self.Colors.communicationPanelColor)
        self.communicationPanel.grid(row=self.Layout.communicationPanelRow, column=0, rowspan=self.Layout.communicationPanelRowSpan,
                                columnspan=self.Layout.maxWidth, sticky="ew", **self.Layout.paddings)

        # Command self.send Bar
        commandSend = tk.Entry(self.communicationPanel, textvariable=self.active_input, width=self.Layout.commandSendWidth)
        commandSend.grid(row=self.Layout.commandSendRow, column=self.Layout.commandSendColumn, rowspan=self.Layout.commandSendRowSpan,
                         columnspan=self.Layout.commandSendColumnSpan,  sticky="ew",
                             **self.Layout.paddings)

        # Command self.send Button
        self.commandSendButton = tk.Button(self.communicationPanel, text="Send", width=self.Layout.commandSendButtonWidth,
                                      bg=self.Colors.communicationPanelColor, command=self.data_send)
        self.commandSendButton.grid(row=self.Layout.commandSendButtonRow, column=self.Layout.commandSendButtonColumn,
                               rowspan=self.Layout.commandSendButtonRowSpan, columnspan=self.Layout.commandSendButtonColumnSpan,
                               sticky="ew", **self.Layout.paddings)

        # Communication Monitor
        self.communicationMonitor = tk.Text(self.communicationPanel, width=self.Layout.dataMonitorWidth,
                                       height=self.Layout.dataMonitorHeight, padx=15, pady=15)
        self.communicationMonitor.grid(row=self.Layout.dataMonitorRow, column=self.Layout.dataMonitorColumn,
                                  rowspan=self.Layout.dataMonitorRowSpan, columnspan=self.Layout.dataMonitorColumnSpan,
                                  sticky="ew", **self.Layout.paddings)
        self.communicationMonitor.insert(tk.END, "Float Messages")
        self.communicationMonitor['state'] = 'disabled'

        # Serial Monitor
        self.serialMonitor = tk.Text(self.communicationPanel, width=self.Layout.serialMonitorWidth, height=self.Layout.serialMonitorHeight,
                                padx=15, pady=15)
        self.serialMonitor.grid(row=self.Layout.serialMonitorRow, column=self.Layout.serialMonitorColumn,
                           rowspan=self.Layout.serialMonitorRow, columnspan=self.Layout.serialMonitorColumnSpan,  sticky="ew",
                             **self.Layout.paddings)
        self.serialMonitor.insert(tk.END, "Serial Monitor")
        self.serialMonitor['state'] = 'disabled'

        # Console
        self.console = tk.Text(self.communicationPanel, width=self.Layout.consoleWidth, height=self.Layout.consoleHeight, padx=15, pady=15)
        self.console.grid(row=self.Layout.consoleRow, column=self.Layout.consoleColumn, rowspan=self.Layout.consoleRowSpan,
                     columnspan=self.Layout.consoleColumnSpan, sticky="ew", **self.Layout.paddings)
        self.console.insert(tk.END, "Console")
        self.console['state'] = 'disabled'

        # Clear Button
        self.clearButton = tk.Button(self.communicationPanel, text="Clear", width=self.Layout.clearButtonWidth,
                                bg=self.Colors.communicationPanelColor,
                                command=lambda: [self.console_clear(), self.serialMonitor_clear()])
        self.clearButton.grid(row=self.Layout.clearButtonRow, column=self.Layout.clearButtonColumn, rowspan=self.Layout.clearButtonRowSpan,
                         columnspan=self.Layout.clearButtonColumnSpan, sticky="ew", **self.Layout.paddings)

        self.communicationPanel.grid_rowconfigure(0, weight=1)
        self.communicationPanel.grid_columnconfigure(0, weight=1)
        # </editor-fold>

        # Set window properties
        self.grid_rowconfigure(self.Layout.maxHeight, weight=1)
        self.grid_columnconfigure(self.Layout.portColumn, weight=1)
        # </editor-fold>

    def start(self):
        self.update_ports()
        self.read()

if __name__ == '__main__':
    # Create instance of UI
    UI = FloatUI()
    # Begin interval functions
    UI.start()
    # Execute Tkinter
    UI.mainloop()
