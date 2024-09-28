print("helleh woow")

import pandas as pd
from clickhouse_driver import Client

# Connect to the ClickHouse instance
client = Client(host='clickhouse', port=9000, user='default', password='')

# Example query
def query_clickhouse():
    query = "SELECT full_name FROM table_dataset1 LIMIT 10"
    data = client.execute(query)
    df = pd.DataFrame(data, columns=['full_name'])
    
    return df

if __name__ == "__main__":
    print("test") 
    q = query_clickhouse()

    print(q)
