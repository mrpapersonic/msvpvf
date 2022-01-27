import tkinter
import tkinter.filedialog
import msvpvf
import os

def round_down(num, divisor):
    return num - (num%divisor)

def open_veg_file():
    filepath.set(tkinter.filedialog.askopenfilename(title="Select project file", filetypes=[("Project files", ".veg .vf"), ("All Files", "*.*")]))
    with open(filepath.get(), 'rb') as file:
        version_of_file.set("Project file version: %s" % (str(bytearray(file.read())[0x46])))
        window.update_idletasks()
    # eventually add shit here

def save_veg_file():
    with open(filepath.get(), 'rb') as f:
        path = bytearray(f.read())
    for i in listbox.curselection():
        prefix, data = msvpvf.type_hex(i)
    path[0x18:0x27] = data
    path[0x46] = int(default_version.get())
    outputfn = prefix + "_V" + default_version.get() + "_" + os.path.basename(os.path.splitext(filepath.get())[0])
    outputfn += ".vf" if prefix == "MS" else ".veg"
    name = tkinter.filedialog.asksaveasfilename(defaultextension='.vf' if prefix == "MS" else '.veg', initialfile=outputfn, filetypes=[("Project files", ".veg .vf"), ("All Files", "*.*")])
    with open(name, 'wb') as f:
        f.write(path)
    

window = tkinter.Tk()
window.geometry("225x200")

filepath = tkinter.StringVar(window)
version_of_file = tkinter.StringVar(window)
selected_version = tkinter.IntVar()


button = tkinter.Button(text="Open", command=open_veg_file)
button.pack(pady=5)
list_items = tkinter.StringVar(value=("VEGAS Pro", "Movie Studio"))
listbox = tkinter.Listbox(window, height=2, listvariable=list_items)
listbox.select_set(0)
listbox.pack(pady=5)

supported_versions = ["8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18"]
default_version = tkinter.StringVar()
default_version.set(supported_versions[3])

menu = tkinter.OptionMenu(window, default_version, *supported_versions)
menu.pack(pady=5)
button = tkinter.Button(text="Save", command=save_veg_file)
button.pack(pady=5)

tkinter.Label(window, textvariable=version_of_file).pack(pady=5)
window.title("msvpvf")
window.minsize(225, 200)

window.mainloop()
