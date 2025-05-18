import mysql.connector as sql
import os
from mysql.connector import Error

def connect_db():
    try:    
        db = sql.connect(
            host = os.getenv("DB_HOST"),
            user = os.getenv("DB_USER"),
            password = os.getenv("DB_PASS"),
            database = "dcu"
        )
        return db
    except Error as e:
        print(f"Error while connecting to MySQL: {e}")
        return None

db = connect_db()
if db is None:
    print("Failed to reconnect to DB. Exiting loop.")
    # break  # or continue, depending on your needs

cursor = db.cursor()

obis_codes = ['0.0.1.0.0.255', '0.0.96.1.0.255', '1.0.32.7.0.255', '1.0.52.7.0.255', '1.0.72.7.0.255', '1.0.31.7.0.255', '1.0.51.7.0.255', '1.0.71.7.0.255', '1.0.91.7.0.255', '1.0.33.7.0.255', '1.0.53.7.0.255', '1.0.73.7.0.255', '1.0.13.7.0.255', '1.0.81.7.0.255', '1.0.81.7.1.255', '1.0.81.7.2.255', '1.0.81.7.4.255', '1.0.81.7.15.255', '1.0.81.7.26.255', '1.0.21.7.0.255', '1.0.41.7.0.255', '1.0.61.7.0.255', '1.0.1.7.0.255', '1.0.22.7.0.255', '1.0.42.7.0.255', '1.0.62.7.0.255', '1.0.2.7.0.255', '1.0.23.7.0.255', '1.0.43.7.0.255', '1.0.63.7.0.255', '1.0.3.7.0.255', '1.0.24.7.0.255', '1.0.44.7.0.255', '1.0.64.7.0.255', '1.0.4.7.0.255', '1.0.29.7.0.255', '1.0.49.7.0.255', '1.0.69.7.0.255', '1.0.9.7.0.255', '1.0.30.7.0.255', '1.0.50.7.0.255', '1.0.70.7.0.255', '1.0.10.7.0.255', '1.0.21.8.0.255', '1.0.41.8.0.255', '1.0.61.8.0.255', '1.0.1.8.0.255', '1.0.22.8.0.255', '1.0.42.8.0.255', '1.0.62.8.0.255', '1.0.2.8.0.255', '1.0.23.8.0.255', '1.0.43.8.0.255', '1.0.63.8.0.255', '1.0.3.8.0.255', '1.0.24.8.0.255', '1.0.44.8.0.255', '1.0.64.8.0.255', '1.0.4.8.0.255', '1.0.128.8.0.255', '1.0.14.7.0.255', '1.0.11.7.124.255', '1.0.11.7.125.255', '0.0.96.6.3.255']
# Sanitize column names (MySQL doesn’t like dots in names)
# safe_columns = [code.replace('.', '_') for code in obis_codes]

# Build SQL CREATE TABLE statement
columns_sql = ', '.join([f'`{col}` VARCHAR(255)' for col in obis_codes])
create_table_sql = f'''
CREATE TABLE IF NOT EXISTS dt_meter_profile_instant (
    id INT AUTO_INCREMENT PRIMARY KEY,
    {columns_sql}
)
'''
try:
    cursor.execute(create_table_sql)
    db.commit()
    print("✅ Table 'dt_meter_profile_instant' created or already exists.")
except Error as e:
    print(f"❌ Failed to create table: {e}")
finally:
    cursor.close()
    db.close()