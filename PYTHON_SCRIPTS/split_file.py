#input is a file path
#data is split 90% training and 10% testing

import pandas as pd
import numpy as np
import os
import sys

import pandas as pd
import numpy as np
import os
import sys
from sklearn.model_selection import train_test_split

# Handle user input validation
if len(sys.argv) < 2:
    print("Usage: python " + sys.argv[0] + " <data_set>")
    sys.exit(1)

# Get file path
try:
    data_path = sys.argv[1]
    if not os.path.exists(data_path):
        raise FileNotFoundError
except FileNotFoundError:
    print("File not found. Please enter a valid file path for <data_set>")
    sys.exit(1)

try:
    df = pd.read_csv(data_path)

    # Possible label column names
    label_columns = ['label', 'labels', 'class', 'classes', 'Label', 'Labels', 'Class', 'Classes', 'species', 'Species']

    # Find the first matching label column
    label_col = next((current_col for current_col in label_columns if current_col in df.columns), None)

    if label_col:
        # Move the label column to the front
        cols = [label_col] + [col for col in df.columns if col != label_col]
        df = df[cols]
    else:
        print("No label column found in the DataFrame. Cannot perform stratified split.")
        sys.exit(1)
except Exception as e:
    print(f"Error reading or processing the file: {e}")
    sys.exit(1)

# Separate features (X) and labels (y)
X = df.drop(columns=[label_col])
y = df[label_col]

# Perform stratified split
# test_size=0.1 means 10% for testing, 90% for training
# stratify=y ensures that the split maintains the proportion of classes in 'y'
# random_state for reproducibility
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.1, stratify=y, random_state=42)

# Recombine X and y for training and testing dataframes
train_df = pd.concat([y_train, X_train], axis=1)
test_df = pd.concat([y_test, X_test], axis=1)

# Save to CSV
train_df.to_csv('train_' + data_path, index=False)
test_df.to_csv('test_' + data_path, index=False)

print(f"Data successfully split into 'train_{data_path}' and 'test_{data_path}' with stratified sampling.")
print(f"Training set size: {len(train_df)}")
print(f"Testing set size: {len(test_df)}")