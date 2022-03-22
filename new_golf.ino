//Can run courseHasher script in noso for course file names

#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <math.h>

LiquidCrystal lcd(7, 8, 9, 10, 5, 6);

File myFile;

int mainMenuPos = 0;
int atdMenuPos = 0;
int srMenuPos = 0;
int prMenuPos = 0;

int state = 0;
int roundCount = 0;
int totalFairways = 0;
int fairwaysHit = 0;
int totalGirs = 0;
int girsHit = 0;
String courseHash = "";

const String choices[] = {"Yes", "No"};

int roundGir[18];
int roundFairway[18];
int roundScore[18];
int roundPutts[18];
int roundYards[18];
int roundPars[18];
int roundStats[2];

char menuOptions[16];
char teeOptions[6];
int previousScores[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; //temp
char courseParArr[18];
int previousPutts[10]; //temp

String averageValues[5] = {"Need 1 Round", "Need 1 Round", "Need 1 Round", "Need 1 Round", "Need 1 Round"}; //temp
String previousCourses[10]; //temp

String selectedCourse = "";
String selectedTees = "";

/* ---------- Helper Functions ---------- */

void ClearMenuOptions() {
  for (int i = 0; i < 16; i++) {
    menuOptions[i] = NULL;
  }
}

void ReadMenuLine(String fileName, int lineCount) {
  ClearMenuOptions();
  int crCount = 0;
  int idx = 0;
  char c;

  myFile = SD.open(fileName);
  while(myFile.available()) {
    c = myFile.read();
    if(c == '\n') {
      crCount++; 
      c = myFile.read();   
    }
    if(crCount == lineCount) {
      if(c != '\r') {
        menuOptions[idx] = c;
        idx++;
      }
    }
  } 
      for(int i = 0; i < 19; i++)
  {
    Serial.print(menuOptions[i]);
  }
  myFile.close();
  myFile.close();
}

void ReadCourseValues(int lineCount, char arr[]) {
  ClearMenuOptions();
  String coursePars = String(courseHash) + "PARS.TXT";
  int crCount = 0;
  int idx = 0;
  int c;

  myFile = SD.open(coursePars);
  while(myFile.available()) {
    c = myFile.read();
    if(c == '\n') {
      crCount++; 
      c = myFile.read();
    }
    if(crCount == lineCount) {
      if(c != '\r') {
        arr[idx] = c;
        idx++;
      }
    }
  } 
  myFile.close();
}

int CheckButtonPushed() {
  int pushedButton = 0;
  if(digitalRead(A1) == 1) {
    pushedButton = 1;
    delay(300);
  } else if(digitalRead(A2) == 1) {
    pushedButton = 2;
    delay(300);
  } else if(digitalRead(A3) == 1) {
    pushedButton = 3;
    delay(300);
  } else if(digitalRead(A4) == 1) {
    pushedButton = 4;
    delay(300);
  }
  return pushedButton;
}

/* ---------- End Helper Functions ---------- */

/* ---------- Menu Navigation Functions ---------- */


int MenuPosition(int navDirection, int menuOptionsLength) {
  int updatedPosition = (navDirection + menuOptionsLength) % menuOptionsLength;
  
  if (state == 0) {
    MainMenuNav(updatedPosition);
  }
  else if (state == 1) {
    CourseMenuNav(updatedPosition);
  }
  else if (state == 2) {
    AtdMenuNav(updatedPosition);
  }
  else if (state == 3) {
    PrMenuNav(updatedPosition);
  }
  return updatedPosition;
}

void TeeMenuNav(int pos) {
  ReadMenuLine(F("TEEOPTS.TXT"), pos);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(F("Choose Tees:"));
  lcd.setCursor(0,1); lcd.write(menuOptions);
}

void MainMenuNav(int pos) {
  ReadMenuLine(F("MMOPTS.TXT"), pos);
  lcd.clear();
  lcd.setCursor(4,0); lcd.print(F("Main Menu"));
  lcd.setCursor(0,1); lcd.write(menuOptions);
}

void CourseMenuNav(int pos) {
  ReadMenuLine(F("COURSES.TXT"), pos);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(F("Select a Course:"));
  lcd.setCursor(0,1); lcd.print(menuOptions);
  selectedCourse = menuOptions;
  GetCourseHash();
}

void AtdMenuNav(int pos) {
  ReadMenuLine(F("AVMOPTS.TXT"), pos);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(menuOptions);
  lcd.setCursor(0,1); lcd.print(averageValues[pos]); 
}

void PrMenuNav(int pos) {
  String prevStr = String(previousCourses[pos]) + " " + String(previousScores[pos]);  
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(F("Previous Rounds"));
  lcd.setCursor(0,1); lcd.print(prevStr);
}

void CrMenuNav(int pos) { 
  lcd.clear();
  bool exitLoop = false;
  while (exitLoop == false) {
    lcd.setCursor(0,0); lcd.print(F("Begin Round At:"));
    lcd.setCursor(0,1); lcd.print(selectedCourse);

    int button = CheckButtonPushed();

    if(button == 1 || button == 2) {
      //Do nothing
    }
    if(button == 3) {
      exitLoop = true;
      state = 1;
      MenuPosition(srMenuPos, 14);
    }
    if(button == 4) {
      exitLoop = true;
      selectedTees = ChooseTees(); //This 
      selectedTees.toUpperCase();
      FetchCourseStats(selectedTees);
    }
  }
}

void FetchCourseStats(String tees) {
  ReadCourseValues(0, courseParArr);
  HoleLogic(courseParArr);
}

void GetCourseHash() {
  char c;
  int courseNameSum = 0;
  
  for (int i = 0; i < 16; i++)
  {
    c = menuOptions[i];
    c = tolower(c);
    courseNameSum = (courseNameSum + (char(c) * i));
  }
  courseHash = String(courseNameSum);
}

String ChooseTees() {
  lcd.clear();
  int tee = 0;
  bool exitLoop = false;
  TeeMenuNav(tee);
  
  while (exitLoop == false){
    int button = CheckButtonPushed();
    
    if(button == 1) {
      if(tee != 0) {
        tee--;
      } else {
        tee = tee + 3;
      }
      TeeMenuNav(tee);
    }
    if(button == 2) {
      if(tee != 3) {
        tee++;
      } else {
        tee = tee - 3;
      }
      TeeMenuNav(tee);
    }
    if(button == 3) {
      exitLoop = true;
      state = 1;
      MenuPosition(srMenuPos, 14);
    }
    if(button == 4) {
      exitLoop = true;
    }
  }
  return menuOptions;
}

/* --------------- End Menu Navigation Functions ---------------*/


/* --------------- Menu Scroll Functions ---------------*/


void PreMenuScroll(int checkButton) {    
  if (state == 0) { //main menu
    MainMenuScroll(checkButton);
  }
  else if (state == 1) { //start round menu
    StartRoundMenuScroll(checkButton);
  }
  else if (state == 2) { //averages menu
    AveragesMenuScroll(checkButton);
  }
  else if (state == 3) { //previous rounds menu
    PreviousRoundMenuScroll(checkButton);
  }
}

void MainMenuScroll(int checkButton) {
  if(checkButton == 1) {
    mainMenuPos = MenuPosition(mainMenuPos - 1, 3);
  }
  else if(checkButton == 2) { 
    mainMenuPos = MenuPosition(mainMenuPos + 1, 3);
  }
  else if(checkButton == 3) {
    //Do nothing
  }
  else {
    if(mainMenuPos == 0) {
      state = 1;
      MenuPosition(srMenuPos, 14);
    }
    else if(mainMenuPos == 1) {
      state = 2;
      MenuPosition(atdMenuPos, 5);
    }
    else {
      state = 3;
      MenuPosition(prMenuPos, 10);
    }
  }
}

void StartRoundMenuScroll(int checkButton) {
  if(checkButton == 1) {
    srMenuPos = MenuPosition(srMenuPos - 1, 14);
  }
  else if(checkButton == 2) {
    srMenuPos = MenuPosition(srMenuPos + 1, 14);
  }
  else if(checkButton == 3) {
    state = 0;
    MenuPosition(mainMenuPos, 3);
  }
  else {
    CrMenuNav(srMenuPos);
    if(checkButton == 1 || checkButton == 2 || checkButton == 4) { 
      //Do nothing
    }
    else { 
      MenuPosition(srMenuPos, 14);
    }  
  }
}

void AveragesMenuScroll(int checkButton) {
  if(checkButton == 1) {
    atdMenuPos = MenuPosition(atdMenuPos - 1, 5);
  }
  else if(checkButton == 2) {
    atdMenuPos = MenuPosition(atdMenuPos + 1, 5);
  }
  else if(checkButton == 3) {
    state = 0;
    MenuPosition(mainMenuPos, 3);
  }
  else {
    //Do nothing
  }
}

void PreviousRoundMenuScroll(int checkButton) {
  if(checkButton == 1) {
    prMenuPos = MenuPosition(prMenuPos - 1, 10);
  }
  else if(checkButton == 2) {
    prMenuPos = MenuPosition(prMenuPos + 1, 10);
  }
  else if(checkButton == 3) {
    state = 0;
    MenuPosition(mainMenuPos, 3);
  }
  else {
    //Do nothing
  }
}


/* --------------- End Menu Scroll Functions ---------------*/

/* --------------- Checker Functions ---------------*/


void FairwayHitCheck(int pos) {
  int postn = 0;
  lcd.clear();
  bool exitLoop = false;
  
  while (exitLoop == false) {
    lcd.setCursor(0,0); lcd.print(F("FAIRWAY HIT?"));
    lcd.setCursor(7,1); lcd.print(choices[postn % 2]);

    int button = CheckButtonPushed();

    if(button == 1 || button == 2) {
      lcd.clear();
      postn++;
    }
    if(button == 3) {
      //Do Nothing
    }
    if(button == 4) {
      lcd.clear();
      exitLoop = true;
      if (choices[postn % 2] == F("Yes")) {
        roundFairway[pos] = 1;
      }
      else {
        roundFairway[pos] = 0;
      }
    }
  }
}

void GirCheck(int pos) {
  int postn = 0;
  lcd.clear();
  bool exitLoop = false;
  
  while (exitLoop == false) {
    lcd.setCursor(0,0); lcd.print(F("GIR HIT?"));
    lcd.setCursor(7,1); lcd.print(choices[postn % 2]);

    int button = CheckButtonPushed();

    if(button == 1 || button == 2) {
      lcd.clear();
      postn++;
    }
    if(button == 3) {
      lcd.clear();
      FairwayHitCheck(pos);   
    }
    
    if(button == 4) {
      lcd.clear();
      exitLoop = true;
      
      if (choices[postn % 2] == F("Yes")) {
        roundGir[pos] = 1;
      }
      else {
        roundFairway[pos] = 0;
      }
    }
  }
}

void PuttsCheck(int pos)
{
  int putts = 1;
  lcd.clear();
  bool exitLoop = false;
  
  while (exitLoop == false) {
    lcd.setCursor(0,0); lcd.print(F("NUMBER OF PUTTS?"));
    lcd.setCursor(8,1); lcd.print(putts);

    int button = CheckButtonPushed();

    if(button == 1) {
      lcd.clear();
      
      if (putts != 0) {
        putts--;
      }
    }
    
    if(button == 2) {
      lcd.clear();
      if (putts < 11) {
        putts++;
      }
    }
    
    if(button == 3) {
      lcd.clear();
      GirCheck(pos); 
    }
    
    if(button == 4) {
      lcd.clear();
      exitLoop = true;
      roundPutts[pos] = putts;
    }
  }
}

void ScoreCheck(int pos) {
  char score = courseParArr[pos];
  lcd.clear();
  bool exitLoop = false;
  
  while (exitLoop == false) {
    lcd.setCursor(0,0); lcd.print(F("HOLE SCORE?"));
    lcd.setCursor(8,1); lcd.print(score);

    int button = CheckButtonPushed();

    if(button == 1) {
      lcd.clear();
      
      if (score != 1) {
        score--;
      }
    }
    
    if(button == 2) {
      lcd.clear();
      score++;
    }
    
    if(button == 3) {
      lcd.clear();
      GirCheck(pos); 
    }
    
    if(button == 4) {
      lcd.clear();
      exitLoop = true;
      roundScore[pos] = score;
    }
  }
}


/* --------------- End Checker Functions ---------------*/

/* --------------- Update Functions ---------------*/


void UpdateAvgScore(int scores[]) {
  float totalScore = 0;
  float averageScore = 0;
  
  for(int i = 0; i < roundCount; i++) {
    if(scores[i] != 0) {
      totalScore = totalScore + scores[i];
    }
  }
  averageScore = (totalScore / roundCount);
  averageValues[1] = String(averageScore) + F(" Strokes");
}

void UpdateAvgFairways(float fwysHit, float fwysTotal) {
  float averageFairways = 0;
  averageFairways = round((fwysHit / fwysTotal) * 100);
  averageValues[2] = String(int(fwysHit)) + F("/") + String(int(fwysTotal)) + F(" - ") + String(round(averageFairways)) + F("%");
}

void UpdateAvgGirs(float grsHit, float grsTotal) {
  float averageGirs = 0;
  averageGirs = round((grsHit / grsTotal) * 100);
  averageValues[3] = String(int(grsHit)) + F("/") + String(int(grsTotal)) + F(" - ") + String(round(averageGirs)) + F("%");
}

void UpdateAvgPutts(int putts[]) {
  float totalPutts = 0;
  float averagePutts = 0;
  float puttCount = 0;
  
  for(int i = 0; i < roundCount; i++) {
    if(putts[i] != 0)
    {
      totalPutts = totalPutts + putts[i];
    }
  }
  averagePutts = (totalPutts / roundCount);
  averageValues[4] = String(averagePutts) + F(" Putts");
}


/* --------------- End Update Functions ---------------*/


/* --------------- Game Logic Functions ---------------*/


void HoleLogic(char pars[]) {
  float totalRoundFairways = 0;
  float totalRoundGirs = 0;
  int rndGir = 0;
  int rndFwy = 0;
  char rndScore = 0;
  int rndPutts = 0;
  int hole = 1;
  int tee = 0;

  if(selectedTees.length() > 4)
  {
    //Need to trim file name size to 8 characters. If colour is more than 4 letters long, truncate it to 4 letters
  }
   
  String holeYards = String(courseHash) + String(selectedTees) + ".TXT";
  
  while(hole < 3) { //19
    ReadMenuLine(holeYards, hole - 1);
    lcd.clear();
    
    lcd.setCursor(0,0);lcd.print(F("Hole: "));
    lcd.setCursor(6,0);lcd.print(hole);
    
    lcd.setCursor(10,0);lcd.print(F("Par: "));
    lcd.setCursor(15,0);lcd.print(pars[hole - 1]); 
      
    lcd.setCursor(0,1);lcd.print(F("Distance: "));
    lcd.setCursor(10,1);lcd.print(menuOptions);
    lcd.setCursor(13,1);lcd.print(F("Y"));
    
    delay(5000);

    if(pars[hole - 1] != 3) {
      FairwayHitCheck(hole - 1);
      totalRoundFairways++;
    }   
    GirCheck(hole - 1);
    PuttsCheck(hole - 1);
    ScoreCheck(hole - 1); 
    totalRoundGirs++;
    hole++;
  }

  lcd.clear();
  lcd.setCursor(0,0); lcd.print(F("Round Complete!"));
  
  for (int i = 0; i < 18; i++) {
    rndScore = rndScore + roundScore[i];
    rndGir = rndGir + roundGir[i];
    rndFwy = rndFwy + roundFairway[i];
    rndPutts = rndPutts+ roundPutts[i];
  }

  delay(5000);
  lcd.clear();
  bool exitLoop = false;
  int posi = 0;
  
  while (exitLoop == false) {
    if (posi == 0) {
      lcd.setCursor(0,0); lcd.print(F("Score:"));
      lcd.setCursor(0,1); lcd.print(int(rndScore));

      previousScores[roundCount] = rndScore;
      previousCourses[roundCount] = selectedCourse;
    }
    else if (posi == 1) {
      String fwyStr = String(rndFwy) + F("/") + String(int(totalRoundFairways)) + F(" - ") + String(round((rndFwy / totalRoundFairways) * 100)) + F("%");
            
      lcd.setCursor(0,0); lcd.print(F("Fairways Hit:"));
      lcd.setCursor(0,1);lcd.print(fwyStr);
    }
    else if (posi == 2) {
      String girStr = String(rndGir) + F("/") + String(int(totalRoundGirs)) + F(" - ") + String(round((rndGir / totalRoundGirs) * 100)) + F("%");
      
      lcd.setCursor(0,0); lcd.print(F("GIRs Hit:"));
      lcd.setCursor(0,1); lcd.print(girStr); 
    }
    else if (posi == 3) {
      lcd.setCursor(0,0); lcd.print(F("Round Putts:"));
      lcd.setCursor(0,1); lcd.print(rndPutts);

      previousPutts[roundCount] = rndPutts;
    }
    else if (posi == 4) {
      lcd.setCursor(0,0); lcd.print(F("Save and return"));
      lcd.setCursor(0,1); lcd.print(F("to main menu?"));
    }
    
    int button = CheckButtonPushed();

    if(button == 1) {
      lcd.clear();
      
      if(posi != 0){
        posi--;
      }
    }
    if(button == 2) {
      lcd.clear();
      
      if(posi < 4) {
        posi++; 
      }
    }
    if(button == 3) {
      //Do nothing
    }
    if(button == 4) {
      if(posi == 4) {
        roundCount++;
        state = 0;
        mainMenuPos = 0;
        atdMenuPos = 0;
        srMenuPos = 0;
        prMenuPos = 0;
        exitLoop = true;
        selectedCourse = "";
        selectedTees = "";

        fairwaysHit = fairwaysHit + rndFwy;
        totalFairways = totalFairways + totalRoundFairways;
        girsHit = girsHit + rndGir;
        totalGirs = totalGirs + totalRoundGirs; 
        
        UpdateAvgScore(previousScores);
        UpdateAvgFairways(fairwaysHit, totalFairways);
        UpdateAvgGirs(girsHit, totalGirs);
        UpdateAvgPutts(previousPutts);

        lcd.clear();
        MainMenuNav(0); //This is broken
      }
    }
  }
}


/* --------------- End Game Logic Functions ---------------*/


void setup() {
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT); 
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(4)) {
    Serial.println(F("initialization failed!"));
    while (1);
  }
  Serial.println(F("initialization done."));

  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(F("POCKET CADDIE>>>"));
  lcd.setCursor(2,1); lcd.print(F("WELCOME ADAM"));
  delay(3000);
  lcd.clear();
  MainMenuNav(0);
}

void loop() {
  int checkBtn = CheckButtonPushed();
  if (checkBtn != 0) {
    PreMenuScroll(checkBtn); 
  }
}
