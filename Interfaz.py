import tkinter as tk
from tkinter import ttk
import serial
import serial.tools.list_ports

ser = None  # Inicializar la variable de la conexión serial

def connect_serial():
    global ser
    try:
        # Especifica aquí el puerto, baudrate, paridad, tamaño de datos y stopbits
        COM_PORT = 'COM8'  # Reemplaza 'COM8' con el puerto adecuado
        BAUDRATE = 9600  # Reemplaza con el baudrate adecuado

        ser = serial.Serial(
            port=COM_PORT,
            baudrate=BAUDRATE,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS
        )
        connect_button.config(text="Conectado", state="disabled")
        print(f"Conectado a {COM_PORT} a {BAUDRATE} baudrate")
    except Exception as e:
        print(f"Error al conectar: {e}")

def send_data():
    global ser
    if ser and ser.is_open:
        data = data_entry.get()
        try:
            # Enviar el dato tal cual está en el campo de texto
            if data.isdigit() and 0 <= int(data) <= 255:
                ser.write(bytes([int(data)]))
                print(f"Enviado: {data}")
            else:
                print("Por favor, ingrese un número entre 0 y 255.")
        except Exception as e:
            print(f"Error al enviar datos: {e}")
    else:
        print("El puerto serial no está abierto o no está conectado.")

def read_serial():
    global ser
    if ser and ser.is_open:
        try:
            while ser.in_waiting:
                line = ser.readline().decode('utf-8').strip()
                if line.startswith("Voltage1:"):
                    value1.config(text=line.split(":")[1].strip())
                elif line.startswith("Voltage2:"):
                    value2.config(text=line.split(":")[1].strip())
        except Exception as e:
            print(f"Error al leer datos: {e}")
    root.after(100, read_serial)  # Llama a esta función nuevamente después de 100 ms

def create_gui():
    global connect_button, data_entry, value1, value2, root
    
    # Crear la ventana principal
    root = tk.Tk()
    root.title("Interfaz de Control")
    root.configure(bg='#f7f7f7')  # Color de fondo de la ventana principal

    # Estilo
    style = ttk.Style()
    style.configure('TFrame', background='#f7f7f7')  # Fondo del marco
    style.configure('TLabel', background='#f7f7f7', font=('Arial', 12))
    style.configure('TButton', font=('Arial', 12), padding=10, background='#5dade2', foreground='black')  # Botones
    style.map('TButton', background=[('active', '#3498db')])

    # Crear un marco para contener los widgets
    frame = ttk.Frame(root, padding="20 20 20 20")
    frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
    frame.columnconfigure(0, weight=1)
    frame.rowconfigure(0, weight=1)

    # Submarcos para los cuadros de texto
    subframe1 = ttk.Frame(frame, padding="5 5 5 5", borderwidth=2, relief="groove")
    subframe1.grid(row=0, column=0, columnspan=2, padx=5, pady=5, sticky=(tk.W, tk.E))
    subframe2 = ttk.Frame(frame, padding="5 5 5 5", borderwidth=2, relief="groove")
    subframe2.grid(row=1, column=0, columnspan=2, padx=5, pady=5, sticky=(tk.W, tk.E))

    # Etiquetas y campos de texto para "valor pot 1" y "valor pot 2"
    label1 = ttk.Label(subframe1, text="Valor Pot 1:")
    label1.grid(row=0, column=0, padx=5, pady=5, sticky=tk.W)
    value1 = ttk.Label(subframe1, text="N/A", width=20, relief="solid")
    value1.grid(row=0, column=1, padx=5, pady=5)

    label2 = ttk.Label(subframe2, text="Valor Pot 2:")
    label2.grid(row=0, column=0, padx=5, pady=5, sticky=tk.W)
    value2 = ttk.Label(subframe2, text="N/A", width=20, relief="solid")
    value2.grid(row=0, column=1, padx=5, pady=5)

    # Campo de entrada de datos con etiqueta
    entry_label = ttk.Label(frame, text="Ingrese dato para enviar:")
    entry_label.grid(row=2, column=0, padx=5, pady=5, sticky=tk.W)
    data_entry = ttk.Entry(frame, width=25)
    data_entry.grid(row=2, column=1, padx=5, pady=5)

    # Botones
    send_button = ttk.Button(frame, text="Enviar", command=send_data)
    send_button.grid(row=3, column=0, padx=5, pady=10, sticky=(tk.W, tk.E))

    connect_button = ttk.Button(frame, text="Conectar", command=connect_serial)
    connect_button.grid(row=3, column=1, padx=5, pady=10, sticky=(tk.W, tk.E))

    # Ajuste del tamaño mínimo de las columnas
    frame.columnconfigure(0, minsize=150)
    frame.columnconfigure(1, minsize=150)

    # Iniciar el bucle principal de la interfaz
    root.after(100, read_serial)  # Inicia la lectura del puerto serial
    root.mainloop()

# Llamar a la función para crear la interfaz gráfica
create_gui()
