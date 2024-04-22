from flask import Flask, request, send_file
import subprocess
import os

app = Flask(__name__)

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return 'No file part', 400
    
    file = request.files['file']
    if file.filename == '':
        return 'No selected file', 400

    file.save(file.filename)
    
    # Run a simple app (e.g., Notepad) after successful upload
    subprocess.Popen(['notepad.exe', file.filename])

    # Save the uploaded file with a .elfproc extension
    new_filename = os.path.splitext(file.filename)[0] + '.elfproc'
    os.rename(file.filename, new_filename)

    return 'File uploaded successfully', 200

@app.route('/download/<filename>', methods=['GET'])
def download_file(filename):
    if os.path.isfile(filename):
        return send_file(filename, as_attachment=True)
    else:
        return 'File not found', 404

if __name__ == '__main__':
    app.run(debug=True)
