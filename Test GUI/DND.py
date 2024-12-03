import tkinter as tk
from tkinterdnd2 import DND_FILES, TkinterDnD
import time
import os
import subprocess

def create_app():
    app = TkinterDnD.Tk()  
    app.title("FastCloud - DND")  
    app.geometry("400x300")  

    bg1 = tk.PhotoImage(file="2.png")  
    label1 = tk.Label(app, image=bg1)  
    label1.place(x=0, y=0)  
    
    def on_drop(event):
        files = app.tk.splitlist(event.data)  
        for file in files:
            file_name = os.path.basename(file)  # Get the base name of the file
            os.system('./a.out ' + file_name)  # Execute a command with the file name
            subprocess.Popen('./server', shell=True)  # Execute the server in background
    
    app.drop_target_register(DND_FILES)  # Register the app as a drop target
    app.dnd_bind('<<Drop>>', on_drop)  # Bind the drop event to the on_drop function

    app.mainloop()
if __name__ == "__main__":
    create_app()



