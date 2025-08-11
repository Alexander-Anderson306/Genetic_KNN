
#moves the labels column to be the firsts column of the dataframe
#sorts the dataframe by the labels column
#adds another column with the gene label
#saves the dataframe as a csv

import pandas as pd
import os
import sys
import subprocess

#handle user input validation
if len(sys.argv) < 3:
    print("Usage: python " + sys.argv[0] + " string <data_set>, int <number_of_genes>, int [random_seed]")
    sys.exit(1)


#get file path
try:
    data_path = sys.argv[1]
    if not os.path.exists(data_path):
        raise FileNotFoundError
except FileNotFoundError:
    print("File not found. Please enter a valid file path for string <data_set>")
    sys.exit(1)

#get number of genes and validate it
try:
    num_genes = int(sys.argv[2])
    #-1 to account for the header
    result = subprocess.run(f'wc -l < {data_path}', shell=True, capture_output=True, text=True)
    line_count = int(result.stdout.strip()) - 1
    if line_count < 1:
        raise EOFError
    if num_genes < 2 or num_genes > line_count:
        raise ValueError
except ValueError:
    print("Invalid input\nPlease enter a valid integer for int <number_of_genes>\nNumber of genes must be between 2 and the number of data points in the file")
    sys.exit(1)
except EOFError:
    print("File is empty or contains only one line\nPlease enter a valid file path for string <data_set>")
    sys.exit(1)
except Exception as e:
    print(f"An error occurred: {e}")
    sys.exit(1)

#4th optional argument is a random seed
if len(sys.argv) > 3:
    try:
        random_seed = int(sys.argv[3])
    except ValueError:
        print("Invalid input\nPlease enter a valid integer for int <random_seed>")
        sys.exit(1)
else:
    random_seed = None

#look for a column called 'label, labels, class' and move them to the first column of the dataframe
try:
    df = pd.read_csv(data_path)

    # Possible label column names
    label_columns = ['label', 'labels', 'class', 'classes', 'Label', 'Labels', 'Class', 'Classes', 'species', 'Species']

    # Find the first matching label column
    label_col = next((current_col for current_col in label_columns if current_col in df.columns), None)

    if label_col:
        # Move the label column to the front
        #checks each col and makes sure it is not the label column
        cols = [label_col] + [col for col in df.columns if col != label_col]
        df = df[cols]
    else:
        print("No label column found in the DataFrame.")
        sys.exit(1)

except Exception as e:
    print(f"Error reading or processing the file: {e}")
    sys.exit(1)

#randomize the order of the dataframe
try:
    df = df.sample(frac=1, random_state=random_seed)
except Exception as e:
    print(f"Error randomizing the dataframe: {e}")
    sys.exit(1)

#now add a column __gene__ to the front of the dataframe
try:
    df.insert(0, "__gene__", "")
except Exception as e:
    print(f"Error inserting new column: {e}")
    sys.exit(1)

#stride the dataframe by num_genes and add the gene label to the __gene__ column
try:
    for i in range(0, len(df)):
        df.iloc[i, 0] = i % num_genes
except Exception as e:
    print(f"Error assigning gene labels: {e}")
    sys.exit(1)

#now sort the dataframe by the __gene__ column
try:
    df = df.sort_values(by=['__gene__'])
except Exception as e:
    print(f"Error sorting the dataframe: {e}")
    sys.exit(1)

#now save the dataframe as a csv
try:
    df.to_csv('preprocessed_' + data_path, index=False)
except Exception as e:
    print(f"Error saving the dataframe: {e}")
    sys.exit(1)