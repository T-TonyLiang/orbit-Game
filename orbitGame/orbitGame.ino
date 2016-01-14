extern "C" {
#include <delay.h>
#include <FillPat.h>
#include <I2CEEPROM.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
}
#include <stdio.h>

// Local Type Definitions
#define DEMO_0 0
#define DEMO_1 2
#define DEMO_2 1
#define DEMO_3 3
#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3


// Global variables
extern int xchOledMax; // defined in OrbitOled.c
extern int ychOledMax; // defined in OrbitOled.c


/**
 * Local variables
 */
char chSwtCur;
char chSwtPrev;
bool fClearOled;
long lBtn2;

char have[] = {0xFF};

// sprite definitions
int xcoSpriteCur;
int ycoSpriteCur;

int xcoSpriteStart = 125; //128 by 32 is full dimensions
int ycoSpriteStart = 14;

int cSpriteWidth = 2;
int cSpriteHeight = 3;

char switches = 0;
int lastScore = 0;
char sprite[][2] = 
  {{0, 1},
   {1, 1},
   {0, 1}};

char* rgBMPSprite0;
char* rgBMPSprite1;
char* rgBMPSprite;
char* rgBMPSIndicator;

int score = 0;

// shield definitions
char spriteShield[][3] = 
  {{0, 1, 1},
   {1, 1, 1},
   {0, 1, 1}};
   
char shieldIndicator[][5] =
  {{ 1, 1, 1, 0, 1},
   { 1, 1, 1, 0, 1}};
int shieldOn;

// block definitions
int xBlock[10];
int yBlock[10];

int xcoBlockStart = 0;
int ycoBlockStart = 16;

int cBlockWidth = 2;
int cBlockHeight = 3;

char rgBMPBlock[] = {0x00, 0xFF, 0xFF};

// game over definitions
int xcoGameOver = 40;
int ycoGameOver = 8;

int timex = 0;
int gmHeight = 18;
int gmWidth = 10;

char gameOver[][10] =
{ {1, 1, 0, 1, 0, 0, 0, 0, 0, 0},
  {1, 1, 0, 1, 0, 0, 1, 0, 0, 0},
  {1, 1, 1, 1, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
  {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 1, 1, 0, 1},
  {0, 1, 1, 1, 0, 0, 1, 1, 0, 1},
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
  {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
  {0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 0, 0, 1, 1, 1, 0},
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
  {1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
  {1, 0, 1, 0, 1, 0, 0, 0, 0, 0},
  {1, 1, 1, 0, 1, 0, 0, 0, 0, 0},};
  
char* gameOverBMP;
  
// screen definintions
int ycoMax = 32;



/**
 * Forward declarations 
 */ 
void DeviceInit();
char CheckSwitches();
void OrbitSetOled();
int startGame();
void movePieces();
void SpriteUp(int xcoUpdate, int ycoUpdate);
void SpriteDown(int xcoUpdate, int ycoUpdate);
void SpriteStop(int xcoUpdate, int ycoUpdate, bool fDir);

char I2CGenTransmit(char * pbData, int cSize, bool fRW, char bAddr);
bool I2CGenIsNotIdle();



/** -------------------------------------------------------------------- */
/**
 * Essential functions
 */
void setup()
{
  DeviceInit();
  // convert the 1's and 0's of the images into a BMP
  gameOverBMP = formattedBitmap((char*)gameOver, gmWidth, gmHeight);
  rgBMPSprite0 = formattedBitmap((char*)sprite, cSpriteWidth, cSpriteHeight);
  rgBMPSprite1 = formattedBitmap((char*)spriteShield, cSpriteWidth + 1, cSpriteHeight);
  rgBMPSIndicator = formattedBitmap((char*)shieldIndicator, 5, 2);
  rgBMPSprite = rgBMPSprite1;
}

void loop()
{
  mode();
}



/** --------------------------------------------------------------------- */
/**
 * Mode functions
 */

/**
 * mode
 *   checks switches to select mode of operation from:
 *     welcome screen
 *     credits
 *     last score
 *     game
 * 
 * parameters: 
 *   none
 * 
 * return:
 *   none  
 */
void mode()
{
  switches = CheckSwitches();
  switch (switches){
  case DEMO_0:
    game();
    break;
  case DEMO_1: 
    welcome();
    break;
  case DEMO_2:
  scoreboard();
    break;
    case DEMO_3:
    credits();
    break;
  }
}

/**
 * welcome
 *   (mode)
 *   welcome screen with prompt to begin game
 *   terminates when switches are no longer selecting "welcome" mode
 * 
 * parameters: 
 *   none
 * 
 * return:
 *   none  
 */
void welcome()
{
  OrbitOledClear();
  while(true)
  {
    char msg[] = "Welcome!";
    char msg2[] =" Flip switches   down to begin\0";
    OrbitOledSetCursor(0, 0);
    OrbitOledPutString(msg);
    OrbitOledSetCursor(50, 0);
    OrbitOledPutString(msg2);
    switches = CheckSwitches();
    if(switches != DEMO_1)
      break;
  } 
}

/**
 * credits
 *   (mode)
 *   displays contributors
 *   terminates when switches no longer select "credits" mode
 * 
 * parameters: 
 *   none
 * 
 * return:
 *   none  
 */
void credits()
{
  OrbitOledClear();
  while(true)
  {
   char msg[] = "Designed By:   \0";
   OrbitOledSetCursor(0, 0);
   OrbitOledPutString(msg);
   OrbitOledSetCursor(50,0);
   OrbitOledPutString(" Sarthak, Jerry, Tony");
   switches = CheckSwitches();
   if(switches != DEMO_3)
    break;
  } 
}

/**
 * scoreboard
 *   (mode)
 *   displays last score obtained and time survived (not really in seconds)
 *   terminates when switches no longer select "scoreboard" mode
 * 
 * parameters: 
 *   none
 * 
 * return:
 *   none  
 */
void scoreboard()
{
  OrbitOledClear();
  while (true)
  {
    char dscore[5];
    char itime[8];
    int dec = timex%100;
    int i = timex/100;
    char msg[] = "Last Score: \0";
    sprintf(dscore, "%d", lastScore);
    char msg2[] = " Time Survived: \0";
    OrbitOledSetCursor(0,0);
    OrbitOledPutString(msg);
    OrbitOledPutString(dscore);
    OrbitOledSetCursor(50,0);
    sprintf(itime, " %d.%d seconds", i, dec);
    OrbitOledPutString(msg2);
    OrbitOledPutString(itime);
    switches = CheckSwitches();
    if (switches != DEMO_2)
    {
      OrbitOledClear();
      return;
    }
  }
}
  
/**
 * game
 *   (mode)
 *   runs the main game loop
 *   restarts when game is lost
 *   terminates when switches no longer select "game" mode
 * 
 * parameters: 
 *   none
 * 
 * return:
 *   none  
 */
void game()
{
    //if there is a collision
  if (startGame())
  {
    // sprite final position
    spriteMove(xcoSpriteCur, ycoSpriteCur);
    // block final position
    for (int i = 0; i < sizeof(xBlock)/sizeof(xBlock[0]); i++)
    {
      blockMove(xBlock[i], yBlock[i]);
    }    
    OrbitOledUpdate();
    
    // game over screen
    OrbitOledClear();
    OrbitOledMoveTo(xcoGameOver, ycoGameOver);
    OrbitOledPutBmp(gmWidth, gmHeight, gameOverBMP);

    OrbitOledUpdate();
    delay(1500);
  }
}



/** ------------------------------------------------------------------- */
/**
 * Helper functions
 */
 
 /**
 * startGame
 *   starts the game and handles accelerometer input
 *   calls methods to move the sprite and blocks
 * 
 * parameters:
 *   none
 *
 * return:
 *   1 if a collision occurs
 *   0 if there is no collision (sprite still alive)
 */
int startGame()
{
  short dataX;
  short dataY;
  short dataZ;
  
  char printVal[10];
  
  char  chPwrCtlReg = 0x2D;
  char  chX0Addr = 0x32;
  char  chY0Addr = 0x34;
  char  chZ0Addr = 0x36;
  
  //char  rgchReadAccl[] = {
  //  0, 0, 0            };
  char  rgchWriteAccl[] = {
    0, 0            };
    
  char rgchReadAccl2[] = {
    0, 0, 0            };
    
    //char rgchReadAccl3[] = {
    //0, 0, 0            };
    
  xcoSpriteCur = xcoSpriteStart;
  ycoSpriteCur = ycoSpriteStart;

  shieldOn = 0;
  score = 0;
  
  int i;
  int shieldGainTimer = 0;
  int haveShield = 0;

  // give blocks random y coordinates
  for (i = 0; i < 10; i++)
  {
    xBlock[i] = 0;
    yBlock[i] = rand() % 28; 
  }
  
  long timeCount = 0;

  int   yDirThreshPos = 8;
  int   yDirThreshNeg = -8;

  bool fDir = true;
  
  /*
   * If applicable, reset OLED
   */
  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;

    /*
     * Enable I2C Peripheral
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /*
     * Set I2C GPIO pins
     */
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    /*
     * Setup I2C
     */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    /* Initialize the Accelerometer
     *
     */
    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    rgchWriteAccl[0] = chPwrCtlReg;
    rgchWriteAccl[1] = 1 << 3;    // sets Accl in measurement mode
    I2CGenTransmit(rgchWriteAccl, 1, WRITE, ACCLADDR);

  }
  
  // draw the starting sprite
  OrbitOledMoveTo(xcoSpriteStart, ycoSpriteStart);
  OrbitOledPutBmp(cSpriteWidth, cSpriteHeight, rgBMPSprite);

  OrbitOledUpdate();
  
  // loop and check for movement
  // MAIN GAME LOOP
  while (true) 
  {
    switches = CheckSwitches();
    if (switches != DEMO_0)
    {
       break; 
    }
    // check state of button (for shield)
    int shieldTimer;
    lBtn2 = GPIOPinRead(BTN2Port, BTN2);
    if (lBtn2 == BTN2 && haveShield)
    {
      shieldOn = 1;
      shieldTimer = 0;
      haveShield = 0;
    }
    // turn off shield when its time is up (2 sec??)
    if (shieldTimer >= 100)
    {
      shieldOn = 0;
      shieldTimer = 200;
    }
    // if 10 seconds passed, gain a shield (max 1)
    if (shieldGainTimer >= 500)
    {
      shieldGainTimer = 0;
      if (!haveShield)
        haveShield++;
    }
    // display shield notification
    if (haveShield)
    {
      OrbitOledMoveTo(0, 0);
      OrbitOledPutBmp(4, 2, rgBMPSIndicator);
      OrbitOledUpdate();
    }

    // read the y data register for the accelerometer
    rgchReadAccl2[0] = chY0Addr;
    I2CGenTransmit(rgchReadAccl2, 2, READ, ACCLADDR);

    dataY = (rgchReadAccl2[2] << 8) | rgchReadAccl2[1];

    if(dataY < 0 && dataY < yDirThreshNeg) {
      fDir = true;

      if(ycoSpriteCur <= 0) {
        ycoSpriteCur = 0;
      }
      else {
        ycoSpriteCur--;
        //OrbitOledClear();
        spriteMove(xcoSpriteCur, ycoSpriteCur);
      }
    }

    else if(dataY > 0 && dataY > yDirThreshPos) {
      fDir = false;
      if(ycoSpriteCur >= (ycoMax - cSpriteHeight - 1)) 
      {
        ycoSpriteCur = ycoMax - cSpriteHeight - 1;
      }
      else 
      {
        ycoSpriteCur++;
        //OrbitOledClear();
        spriteMove(xcoSpriteCur, ycoSpriteCur);
      }
    }
    
    ///////////////////////////////////
    // FALLING PIECES
    OrbitOledClear();
    spriteMove(xcoSpriteCur, ycoSpriteCur);
    
    blockMove(xBlock[0]++, yBlock[0]); // also have if timecount & no collision when implemented
    if (timeCount > 570)
    {
      blockMove(xBlock[1]++, yBlock[1]);
    }
    if (timeCount > 1040)
    {
      blockMove(xBlock[2]++, yBlock[2]);
      blockMove(xBlock[3]++, yBlock[3]);
      xBlock[3]++;
    }
    if (timeCount > 1330)
    {
      blockMove(xBlock[4]++, yBlock[4]);
      blockMove(xBlock[5], yBlock[5]);
      xBlock[5] += 2;
      
    }
    if (timeCount > 1680)
    {
      blockMove(xBlock[6]++, yBlock[6]);
      blockMove(xBlock[7]++, yBlock[7]);
      xBlock[7] += 2;
    }
    
    for (i = 0; i < 10; i ++)
    {
      if (xBlock[i] > 127)
      {
         xBlock[i] = 0; 
         yBlock[i] = rand() % 28;
         score ++;
      }
    }
    // END FALLING PIECES
    //////////////////////////////
    
    // done after blocks and sprite are done updating
    OrbitOledUpdate(); 
    
    ///////////////////////////////
    // CHECK COLLISIONS
    int check = checkCollision(xBlock, yBlock, 
      xcoSpriteCur, ycoSpriteCur, sizeof(xBlock)/sizeof(xBlock[0]));
    if (check){
      lastScore = score;
      timex = timeCount;
      return 1;
    }
    // END CHECK COLLISIONS
    //////////////////////////////
    
    delay(10);
    timeCount++;
    shieldTimer++;
    shieldGainTimer++;
    // the spritestop function isnt doing anything in reality
    /*else {
      SpriteStop(xcoSpriteCur, ycoSpriteCur, fDir);
    }*/
  }
  
  return 0;
}

/**
 * CheckSwitches
 *   checks the state of the boosterpack switches 
 * 
 * parameters:
 *   none
 *
 * return:
 *   character representing the current switch state
 */
char CheckSwitches() {

  long  lSwt1;
  long  lSwt2;

  chSwtPrev = chSwtCur;

  lSwt1 = GPIOPinRead(SWT1Port, SWT1);
  lSwt2 = GPIOPinRead(SWT2Port, SWT2);

  chSwtCur = (lSwt1 | lSwt2) >> 6;

  if(chSwtCur != chSwtPrev) {
    fClearOled = true;
  }
  return chSwtCur;
}

/**
 * spriteMove
 *   Places the new position of the sprite on the Oled screen.
 *
 * paramters:
 *   xcoUpdate the new x-coordinate of the sprite
 *   ycoUpdate the new y-coordinate of the sprite
 *
 * return: 
 *   none
 */
void spriteMove(int xcoUpdate, int ycoUpdate) 
{
  OrbitOledMoveTo(xcoUpdate - shieldOn, ycoUpdate);
  OrbitOledPutBmp(cSpriteWidth + shieldOn, cSpriteHeight + shieldOn, rgBMPSprite);
}

/**
 * blockMove
 *   Places the new position of a block on the Oled screen.
 *
 * paramters:
 *   xcoUpdate the new x-coordinate of the block
 *   ycoUpdate the new y-coordinate of the block
 *
 * return: 
 *   none
 */
void blockMove(int xcoUpdate, int ycoUpdate) {
  OrbitOledMoveTo(xcoUpdate, ycoUpdate);
  OrbitOledPutBmp(cBlockWidth, cBlockHeight, rgBMPBlock);
}

/**
 * checkCollision
 *   checks if a block has collided with the sprite at the new coordinates
 *   draws the last position of the blocks and sprite
 *
 * paramters:
 *   xBlock an array of the new x coodinates of the blocks
 *   yBlock an array of the new y coodinates of the blocks
 *   xcoSpriteCur the new x coodinate of the sprite
 *   ycoSpriteCur the new y coodinate of the sprite
 *   n the length of the xBlock and yBlock arrays
 *
 * return: 
 *   1 if there is a collision between sprite and block for their new positions
 *   0 otherwise
 */
int checkCollision(int xBlock[], int yBlock[], int xcoSpriteCur, int ycoSpriteCur, int n)
{
  for (int i = 0; i < n; i++)
  {
    if ((xBlock[i] >= xcoSpriteCur && yBlock[i] >= ycoSpriteCur - 2 && yBlock[i] <= ycoSpriteCur + 2)
      || (xBlock[i] >= xcoSpriteCur - 1 && yBlock[i] >= ycoSpriteCur - 1 && yBlock[i] <= ycoSpriteCur + 1))
    {
      if (shieldOn)
      {
        xBlock[i] = 0;
        shieldOn = 0;
        return 0;
      }
      else
        return 1;
    }
  }
  return 0;
}

/**
 * Code to convert from 0 and 1 array provided by Kenneth Sinder
 */
char* formattedBitmap(char* input, unsigned int width, unsigned int height)
{
  
  unsigned int h = ceil(height / 8.0);
  char *output = (char*)calloc(h * width, sizeof(char));
  char b, temp;
  for (unsigned int hbyte = 0; hbyte < h; ++hbyte) {
    for (unsigned int i = 0; i < width; ++i) {
      b = 0;
      for (unsigned int j = 0; j < ((height - hbyte * 8)/8 ? 8 : (height%8)); ++j) {
        temp = input[(8*hbyte+j)*width+i];
        if (temp) b |= 1 << j;
      }

      output[hbyte*width+i]|=b;
    }
  }
  return output;
}



/** ------------------------------------------------------------------------ */
/**
 * Technical functions 
 */

/***	DeviceInit
 **
 **	Parameters:
 **		none
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Initialize I2C Communication, and GPIO
 */
void DeviceInit()
{
  /*
   * First, Set Up the Clock.
   * Main OSC		  -> SYSCTL_OSC_MAIN
   * Runs off 16MHz clock -> SYSCTL_XTAL_16MHZ
   * Use PLL		  -> SYSCTL_USE_PLL
   * Divide by 4	  -> SYSCTL_SYSDIV_4
   */
  SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_4);

  /*
   * Enable and Power On All GPIO Ports
   */
  //SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOA | SYSCTL_PERIPH_GPIOB | SYSCTL_PERIPH_GPIOC |
  //						SYSCTL_PERIPH_GPIOD | SYSCTL_PERIPH_GPIOE | SYSCTL_PERIPH_GPIOF);

  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOA );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOB );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOC );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOD );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOE );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOF );
  /*
   * Pad Configure.. Setting as per the Button Pullups on
   * the Launch pad (active low).. changing to pulldowns for Orbit
   */
  GPIOPadConfigSet(SWTPort, SWT1 | SWT2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(BTN1Port, BTN1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
  GPIOPadConfigSet(BTN2Port, BTN2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(LED1Port, LED1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED2Port, LED2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED3Port, LED3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED4Port, LED4, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

  /*
   * Initialize Switches as Input
   */
  GPIOPinTypeGPIOInput(SWTPort, SWT1 | SWT2);

  /*
   * Initialize Buttons as Input
   */
  GPIOPinTypeGPIOInput(BTN1Port, BTN1);
  GPIOPinTypeGPIOInput(BTN2Port, BTN2);

  /*
   * Initialize LEDs as Output
   */
  GPIOPinTypeGPIOOutput(LED1Port, LED1);
  GPIOPinTypeGPIOOutput(LED2Port, LED2);
  GPIOPinTypeGPIOOutput(LED3Port, LED3);
  GPIOPinTypeGPIOOutput(LED4Port, LED4);

  /*
   * Enable ADC Periph
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

  GPIOPinTypeADC(AINPort, AIN);

  /*
   * Enable ADC with this Sequence
   * 1. ADCSequenceConfigure()
   * 2. ADCSequenceStepConfigure()
   * 3. ADCSequenceEnable()
   * 4. ADCProcessorTrigger();
   * 5. Wait for sample sequence ADCIntStatus();
   * 6. Read From ADC
   */
  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
  ADCSequenceEnable(ADC0_BASE, 0);

  /*
   * Initialize the OLED
   */
  OrbitOledInit();

  /*
   * Reset flags
   */
  chSwtCur = 0;
  chSwtPrev = 0;
  fClearOled = true;
  //piece block[50];
  /*for (int i = 0; i < 50; i++)
  {
    block[i].x = -1;
    block[i].y = -1;
  }*/
}

/***	I2CGenTransmit
 **
 **	Parameters:
 **		pbData	-	Pointer to transmit buffer (read or write)
 **		cSize	-	Number of byte transactions to take place
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Transmits data to a device via the I2C bus. Differs from
 **		I2C EEPROM Transmit in that the registers in the device it
 **		is addressing are addressed with a single byte. Lame, but..
 **		it works.
 **
 */
char I2CGenTransmit(char * pbData, int cSize, bool fRW, char bAddr) {

  int 		i;
  char * 		pbTemp;

  pbTemp = pbData;

  /*Start*/

  /*Send Address High Byte*/
  /* Send Write Block Cmd*/
  I2CMasterSlaveAddrSet(I2C0_BASE, bAddr, WRITE);
  I2CMasterDataPut(I2C0_BASE, *pbTemp);

  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

  delay(1);

  /* Idle wait*/
  while(I2CGenIsNotIdle());

  /* Increment data pointer*/
  pbTemp++;

  /*Execute Read or Write*/

  if(fRW == READ) {

    /* Resend Start condition
	** Then send new control byte
	** then begin reading
	*/
    I2CMasterSlaveAddrSet(I2C0_BASE, bAddr, READ);

    while(I2CMasterBusy(I2C0_BASE));

    /* Begin Reading*/
    for(i = 0; i < cSize; i++) {

      if(cSize == i + 1 && cSize == 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

        delay(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else if(cSize == i + 1 && cSize > 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

        delay(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else if(i == 0) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

        delay(1);

        while(I2CMasterBusy(I2C0_BASE));

        /* Idle wait*/
        while(I2CGenIsNotIdle());
      }
      else {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

        delay(1);

        while(I2CMasterBusy(I2C0_BASE));

        /* Idle wait */
        while(I2CGenIsNotIdle());
      }

      while(I2CMasterBusy(I2C0_BASE));

      /* Read Data */
      *pbTemp = (char)I2CMasterDataGet(I2C0_BASE);

      pbTemp++;

    }

  }
  else if(fRW == WRITE) {

    /*Loop data bytes */
    for(i = 0; i < cSize; i++) {
      /* Send Data */
      I2CMasterDataPut(I2C0_BASE, *pbTemp);

      while(I2CMasterBusy(I2C0_BASE));

      if(i == cSize - 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

        delay(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

        delay(1);

        while(I2CMasterBusy(I2C0_BASE));

        /* Idle wait */
        while(I2CGenIsNotIdle());
      }

      pbTemp++;
    }

  }

  /*Stop*/

  return 0x00;

}

/***	I2CGenIsNotIdle()
 **
 **	Parameters:
 **		pbData	-	Pointer to transmit buffer (read or write)
 **		cSize	-	Number of byte transactions to take place
 **
 **	Return Value:
 **		TRUE is bus is not idle, FALSE if bus is idle
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Returns TRUE if the bus is not idle
 **
 */
bool I2CGenIsNotIdle() {

  return !I2CMasterBusBusy(I2C0_BASE);

}



/** ------------------------------------------------------------------------ */
/**
 * Deprecated functions
 */ 

/***  SpriteStop
 **
 ** Parameters:
 **   none
 **
 ** Return Value:
 **   none
 **
 ** Errors:
 **   none
 **
 ** Description:
 **   Keeps the Rocket in one place on the OLED display
 **
 */
void SpriteStop(int xcoUpdate, int ycoUpdate, bool fDir) {
  if(fDir) {
    OrbitOledMoveTo(xcoUpdate, ycoUpdate);
    OrbitOledSetFillPattern(OrbitOledGetStdPattern(0));
    OrbitOledFillRect(xcoUpdate - 1, ycoUpdate);
  }
  else {
    OrbitOledMoveTo(xcoUpdate, ycoUpdate);
    OrbitOledSetFillPattern(OrbitOledGetStdPattern(0));
    OrbitOledFillRect(xcoUpdate, ycoUpdate);
  }

  OrbitOledUpdate();
}

void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
 
 void itoah(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  
         n = -n;        
     i = 0;
     do {     
         s[i++] = n % 10 + '0'; 
     } while ((n /= 10) > 0);    
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }