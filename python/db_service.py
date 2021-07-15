import mysql.connector

mydb = mysql.connector.connect(
  host="10.26.1.16",
  user="lamppost-autopilot",
  password="netcod99",
  database="lamppost_runtime"
)

mycursor = mydb.cursor()

mycursor.execute("SELECT x,y FROM Route ORDER BY pace_id")

myresult = mycursor.fetchall()


pathX = []
pathY = []

for x in myresult:
    pathX.append(x[0])
    pathY.append(x[1]) 