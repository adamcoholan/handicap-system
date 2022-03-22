#Idea behind this script is to put in a golf course name, and based on its name, give a value 
#that can be used as the courses file name

exitLoop = False
courseName = ""

while(1):
  while(exitLoop != True):
    courseName = input("Enter a course name: ")
    print()
    confirm = input("Did you mean to write the name as " + courseName + "? (y/n): ")

    if (confirm.lower() == "y"):
      exitLoop = True

  courseName = courseName.lower()
  count = 0
  courseNameSum = 0

  for c in courseName:
    courseNameSum = (courseNameSum + (ord(c) * count)) 
    count = count + 1
  print()
  print(courseName + ": " + str(courseNameSum))
  print()
  exitLoop = False
