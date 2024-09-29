import pandas as pd
from clickhouse_driver import Client
from filters import normalize_name
from LSH import MinHashLSH
import subprocess
import os

# Connect to the ClickHouse instance
client = Client(host='clickhouse', port=9000, user='default', password='')

# # Example query
# def query_clickhouse():
#     query = "SELECT full_name FROM table_dataset1 LIMIT 10000"
#     data = client.execute(query)
#     df = pd.DataFrame(data, columns=['Name'])
    
#     return df

if __name__ == "__main__":


    # print('querying the first db...')
    # data = query_clickhouse()
    # print(data)

    data = pd.read_csv("input_data\main1.csv")['full_name']

    # очистка
    print("filtering data...")
    # data['Name'] = data["Name"].apply(normalize_name)
    data = data['full_name']
    # print(data)
    
    print("processing...")


    script_dir = os.path.dirname(os.path.abspath(__file__))
    bin_path = os.path.join(script_dir, r"bin\test.exe")


    process = subprocess.Popen(
        [bin_path],  # Path to the C executable
        stdin=subprocess.PIPE,     # Open pipe for stdin
        stdout=subprocess.PIPE,    # Open pipe for stdout
        stderr=subprocess.PIPE     # Open pipe for stderr
    )

    # Send input to the C program's stdin

    data_len = len(data)
    
    print(f"sending {data_len} strings")

    process.stdin.write(str(data_len).encode())

    for i in range(data_len):
        print(data[i])
        process.stdin.write(data[i].encode())
    
    
    process.stdin.flush()  # Ensure input is sent immediately

    # Read the C program's response
    output = process.stdout.readline().decode().strip()
    print(f"Received from C program: {output}")

    # Check for any errors
    stderr = process.stderr.read().decode().strip()
    if stderr:
        print(f"Error: {stderr}")

    # Close the subprocess
    process.stdin.close()
    process.stdout.close()
    process.stderr.close()
    process.wait()
