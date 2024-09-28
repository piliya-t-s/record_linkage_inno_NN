import pandas as pd
from clickhouse_driver import Client
from .filters import normalize_name
from .LSH import MinHashLSH

# Connect to the ClickHouse instance
client = Client(host='clickhouse', port=9000, user='default', password='')

# Example query
def query_clickhouse():
    query = "SELECT full_name FROM table_dataset1 LIMIT 10000"
    data = client.execute(query)
    df = pd.DataFrame(data, columns=['Name'])
    
    return df

if __name__ == "__main__":

    print('querying the first db...')
    data = query_clickhouse()
    print(data)

    # очистка
    print("filtering data...")
    data['Name'] = data["Name"].apply(normalize_name)
    data = data['Name']
    print(data)
    
    print("processing...")
    lsh = MinHashLSH(num_hash_functions=100, num_bands=20, threshold=0.6)

    for s in data:
        lsh.add_string(s)

    similar_groups = lsh.find_similar_groups()

    print("Similar groups (by index):")
    for group in similar_groups:
        print(group)

    print("\nSimilar groups (by string):")
    for group in similar_groups:
        print([data[i] for i in group])


