# ETL-Pipeline

This repository demonstrates a custom ETL (Extract, Transform, Load) pipeline written in C++. It ingests time-series data (e.g., from CSV files), applies transformations, and stores the results in a double-hashing data structure for efficient lookups. The code emphasizes dynamic memory management, robust collision resolution, and modular design to facilitate scalable data processing and analytics.

# Key Features
1. Secure Data Retrieval: Can be adapted to run over a VPN (e.g., Cisco Secure Client) or Linux environments for controlled data transfers.
2. Double-Hashing Storage: Implements a secondary hashing technique to minimize collisions, maintaining fast insertion and retrieval times.
3. Dynamic Memory Management: Uses resizeable arrays and careful pointer handling to accommodate expanding datasets without performance bottlenecks.
4. Modular Architecture: Separates hashing, data parsing, and analytics logic into distinct components, improving readability and maintainability.
5. Basic Analytics: Includes example functions (e.g., range queries, find operations) that demonstrate how to compute aggregated statistics on the loaded data.

# How It Works
1. Extract: Reads raw data from external files or streams, parsing it into in-memory structures.
2. Transform: Cleans and processes each record, converting text inputs into numeric or standardized forms.
3. Load: Inserts the processed data into a collision-resistant hash table, enabling efficient searching, deletion, and updates.
