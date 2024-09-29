import pandas as pd
from clickhouse_driver import Client
from .filters import normalize_name
from .LSH import MinHashLSH
import subprocess
import os
import subprocess
import threading
import time

def read_output(process):
    while True:
        output = process.stdout.readline()
        if output:
            print(f"{output}")
        else:
            break

# Connect to the ClickHouse instance
client = Client(host='clickhouse', port=9000, user='default', password='')

def query_clickhouse():
    query = "SELECT full_name FROM table_dataset1 LIMIT 10000"
    data = client.execute(query)
    df = pd.DataFrame(data, columns=['Name'])
    
    return df

if __name__ == "__main__":


    print('querying the first db...')
    data = query_clickhouse()
    print(data)

    process = subprocess.Popen(
        ['src/bin/run'],  
        stdin=subprocess.PIPE,  
        stdout=subprocess.PIPE,  
        stderr=subprocess.PIPE,
        text=True, 
        bufsize=1, 
    )


    output_thread = threading.Thread(target=read_output, args=(process,))
    output_thread.start()


    process.stdin.write(str(100) + "\n")
    process.stdin.write(str(5) + "\n")
    process.stdin.write(str(0.7) + "\n")
    process.stdin.write(str(len(data['Name'])) + "\n")

    for i in data['Name']:
        process.stdin.write(i + "\n")
        process.stdin.flush()
        
    process.stdin.flush()

    time.sleep(1)


    process.stdin.close() 
    output_thread.join() 
    process.wait() 
