/*
 * Mandalorian Rangefinder HUD Display
 * Waveshare 1.51" Transparent OLED (128x64)
 * 
 * Required Library: U8g2 (install via Arduino Library Manager)
 * 
 * Wiring (for Arduino Uno/Nano):
 * VCC -> 3.3V
 * GND -> GND
 * DIN -> Pin 11 (MOSI)
 * CLK -> Pin 13 (SCK)
 * CS  -> Pin 10
 * DC  -> Pin 9
 * RST -> Pin 8
 */

#include <U8g2lib.h>
#include <SPI.h>

// Pin definitions
#define OLED_CS   10
#define OLED_DC   9
#define OLED_RST  8

// Initialize display
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(U8G2_R0, OLED_CS, OLED_DC, OLED_RST);

int frame = 0;
int targetX = 64;
int targetY = 32;
int scanLine = 0;
bool targetLocked = false;
int lockCounter = 0;

void setup() {
  u8g2.begin();
  u8g2.setContrast(255);
  randomSeed(analogRead(0));
}

void loop() {
  u8g2.clearBuffer();
  
  // Draw main HUD elements
  drawReticle();
  drawTargetingBox();
  drawScanLines();
  drawCornerBrackets();
  drawRangeInfo();
  drawStatusText();
  drawRadarSweep();
  
  u8g2.sendBuffer();
  
  frame++;
  scanLine = (scanLine + 2) % 64;
  
  // Simulate target movement and locking
  if(frame % 60 == 0) {
    targetX = random(20, 108);
    targetY = random(20, 44);
  }
  
  if(frame % 120 == 0) {
    targetLocked = !targetLocked;
    lockCounter = 0;
  }
  
  if(targetLocked) lockCounter++;
  
  delay(50);
}

// Central crosshair reticle
void drawReticle() {
  int cx = 64;
  int cy = 32;
  
  // Center cross
  u8g2.drawLine(cx - 8, cy, cx - 2, cy);
  u8g2.drawLine(cx + 2, cy, cx + 8, cy);
  u8g2.drawLine(cx, cy - 8, cx, cy - 2);
  u8g2.drawLine(cx, cy + 2, cx, cy + 8);
  
  // Center dot
  u8g2.drawPixel(cx, cy);
  
  // Outer circle segments
  for(int angle = 0; angle < 360; angle += 30) {
    float rad = angle * 0.0174533;
    int x1 = cx + cos(rad) * 15;
    int y1 = cy + sin(rad) * 15;
    int x2 = cx + cos(rad) * 18;
    int y2 = cy + sin(rad) * 18;
    u8g2.drawLine(x1, y1, x2, y2);
  }
}

// Moving targeting box
void drawTargetingBox() {
  int size = targetLocked ? 12 : 14;
  
  // Main box
  u8g2.drawFrame(targetX - size, targetY - size, size * 2, size * 2);
  
  // Corner markers
  int corner = 4;
  // Top-left
  u8g2.drawLine(targetX - size - 2, targetY - size - 2, targetX - size + corner, targetY - size - 2);
  u8g2.drawLine(targetX - size - 2, targetY - size - 2, targetX - size - 2, targetY - size + corner);
  // Top-right
  u8g2.drawLine(targetX + size + 2, targetY - size - 2, targetX + size - corner, targetY - size - 2);
  u8g2.drawLine(targetX + size + 2, targetY - size - 2, targetX + size + 2, targetY - size + corner);
  // Bottom-left
  u8g2.drawLine(targetX - size - 2, targetY + size + 2, targetX - size + corner, targetY + size + 2);
  u8g2.drawLine(targetX - size - 2, targetY + size + 2, targetX - size - 2, targetY + size - corner);
  // Bottom-right
  u8g2.drawLine(targetX + size + 2, targetY + size + 2, targetX + size - corner, targetY + size + 2);
  u8g2.drawLine(targetX + size + 2, targetY + size + 2, targetX + size + 2, targetY + size - corner);
  
  // Lock indicator
  if(targetLocked) {
    u8g2.drawLine(targetX - size, targetY, targetX + size, targetY);
    u8g2.drawLine(targetX, targetY - size, targetX, targetY + size);
  }
}

// Scanning lines effect
void drawScanLines() {
  // Horizontal scan line
  u8g2.drawLine(0, scanLine, 128, scanLine);
}

// Screen corner brackets
void drawCornerBrackets() {
  int len = 8;
  // Top-left
  u8g2.drawLine(0, 0, len, 0);
  u8g2.drawLine(0, 0, 0, len);
  // Top-right
  u8g2.drawLine(128 - len, 0, 127, 0);
  u8g2.drawLine(127, 0, 127, len);
  // Bottom-left
  u8g2.drawLine(0, 64 - len, 0, 63);
  u8g2.drawLine(0, 63, len, 63);
  // Bottom-right
  u8g2.drawLine(128 - len, 63, 127, 63);
  u8g2.drawLine(127, 64 - len, 127, 63);
}

// Range and target info
void drawRangeInfo() {
  u8g2.setFont(u8g2_font_4x6_tr);
  
  // Distance readout
  char dist[10];
  int range = 50 + (frame % 50);
  sprintf(dist, "RNG:%dm", range);
  u8g2.drawStr(2, 6, dist);
  
  // Coordinates
  char coords[12];
  sprintf(coords, "X:%d Y:%d", targetX, targetY);
  u8g2.drawStr(2, 13, coords);
  
  // Elevation angle
  u8g2.drawStr(92, 6, "ELV:+2");
  
  // Azimuth
  u8g2.drawStr(92, 13, "AZM:45");
}

// Status text
void drawStatusText() {
  u8g2.setFont(u8g2_font_4x6_tr);
  
  if(targetLocked && lockCounter > 10) {
    // Blinking LOCK text
    if((frame / 5) % 2 == 0) {
      u8g2.drawStr(50, 59, "**LOCKED**");
    }
  } else {
    u8g2.drawStr(48, 59, "SCANNING");
  }
  
  // Bottom corner info
  u8g2.drawStr(2, 59, "TGT");
  u8g2.drawStr(108, 59, "ACT");
}

// Mini radar sweep
void drawRadarSweep() {
  int radarX = 15;
  int radarY = 50;
  int radarR = 8;
  
  // Radar circle
  u8g2.drawCircle(radarX, radarY, radarR);
  u8g2.drawCircle(radarX, radarY, radarR / 2);
  
  // Sweep line
  float angle = (frame * 0.1);
  int sweepX = radarX + cos(angle) * radarR;
  int sweepY = radarY + sin(angle) * radarR;
  u8g2.drawLine(radarX, radarY, sweepX, sweepY);
  
  // Target blip
  int blipX = radarX + cos(angle + 1) * 5;
  int blipY = radarY + sin(angle + 1) * 5;
  u8g2.drawPixel(blipX, blipY);
  u8g2.drawPixel(blipX + 1, blipY);
}
