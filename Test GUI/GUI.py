import tkinter as tk
import os
import time
from time import sleep



def main():
    root = tk.Tk()
    root.title("FastCloud - Client")
    root.geometry("400x300")
    
    test = tk.PhotoImage(file="main.png")
    label1 = tk.Label(root, image=test)
    label1.place(x=0, y=0)
    def d_self():
        os.system("python3 DND.py")
        
        button.master.destroy()  # 'master' is the implicit root Tk object for 'button'
        print('this should print after mainloop is ended')
        


    button = tk.Button(None, text='Continue', command=d_self, bd=0)
    button.pack()
    
    
    root.mainloop()
    
    
    exit(0)
    
    

    

if __name__ == "__main__":
    main()
    
  

