import tkinter as tk
from tkinter import filedialog
import requests

class UploadApp:
    def __init__(self, master):
        self.master = master
        self.master.title("File Upload")

        self.upload_button = tk.Button(master, text="Upload File", command=self.upload_file)
        self.upload_button.pack(pady=10)

    def upload_file(self):
        file_path = filedialog.askopenfilename()
        if file_path:
            url = 'http://localhost:5000/upload'
            files = {'file': open(file_path, 'rb')}
            response = requests.post(url, files=files)
            print(response.text)

def main():
    root = tk.Tk()
    app = UploadApp(root)
    root.mainloop()

if __name__ == "__main__":
    main()
