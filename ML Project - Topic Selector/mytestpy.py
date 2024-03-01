import pandas as pd
import numpy as np
from sklearn.manifold import TSNE
import matplotlib.pyplot as plt

# Load your data from the CSV file
# Assuming your CSV file has headers, adjust the header parameter accordingly
df = pd.read_csv('wparametersfinal.csv', header=None)

# Extract the data from the DataFrame
data = df.values.T  # Transpose to have each vector as a row

# Use t-SNE with a lower perplexity value
tsne = TSNE(n_components=2, perplexity=5, random_state=42)
embedded_data = tsne.fit_transform(data)

# Plot the results for each example
for i in range(embedded_data.shape[0]):
    plt.scatter(embedded_data[i, 0], embedded_data[i, 1], label=f'Example {i + 1}')

plt.title('t-SNE Visualization of Vectors from CSV')
plt.legend()
plt.show()
