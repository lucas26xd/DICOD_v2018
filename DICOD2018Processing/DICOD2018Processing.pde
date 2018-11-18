import processing.serial.*;
import ddf.minim.*;

Serial porta;

PImage imgCedula;

Minim minim; 
AudioPlayer song; 

String buff = "", valCedula = "", audio = "", img = "", msg = "";
int r, g, b;
boolean cor = false, cedula = false, iniciou = false;

void setup() { 
  size(900,600);
  
  porta = new Serial(this, "/dev/ttyUSB0", 9600);
  porta.bufferUntil('\n');
  
  /*textSize(12);
  textAlign(RIGHT);
  text("Trabalho Final de CTS 2018.2", 895, 595);
  delay(2000);*/
  
  minim = new Minim(this); 
  frameRate(2);
}

void draw(){
  if(millis() < 4000 && !iniciou){
    background(random(0, 200), random(0, 200), random(0, 200));
    
    textSize(30);
    textAlign(CENTER);
    text("DICOD", 450, 280);
    textSize(16);
    textAlign(CENTER);
    text("Versão 2.0", 450, 300);
  }else{
    if(!iniciou){
      frameRate(60);
      porta.write(0); //hand-shake
      iniciou = true;
    }else{
      if(cor){
        cor = false;
        background(r, g, b);
        color c = color(r, g, b);
        float brilho = brightness(c);
        
        if(brilho < 127)
          fill(255);
        else
          fill(0);
        
        textSize(18);
        textAlign(LEFT);
        text("R = " + r, 800, 450);
        text("G = " + g, 800, 470);
        text("B = " + b, 800, 490);
        
        executa(audio);
      }else if(cedula){
        cedula = false;
        background(random(0, 200), random(0, 200), random(0, 200));
        textSize(24);
        textAlign(CENTER);
        text(valCedula, 450, 100);
        
        imgCedula = loadImage("img/" + img);
        image(imgCedula, (width / 2) - (imgCedula.width / 2), (height/2) - (imgCedula.height / 2));
        
        executa(audio);
      }else if(!msg.equals("")){
        background(random(0, 200), random(0, 200), random(0, 200));
        textSize(24);
        textAlign(CENTER);
        text(msg, 450, 280);
        msg = "";
      }
    }
  }
  textSize(12);
  textAlign(RIGHT);
  text("Trabalho Final de CTS 2018.2", 895, 595);
  textAlign(LEFT);
  text("DICOD V2.0", 5, 595);
}

void serialEvent(Serial porta) { 
  buff = porta.readStringUntil('\n');
  print(buff);
  String s;
  
  if(buff.indexOf("(cor") >= 0) { //Está recebendo cores (cor;255,243,245,branco)
    //s = buff.substring(buff.indexOf("(")+1, buff.indexOf(";"));
    cor = true;
    buff = buff.substring(buff.indexOf(";")+1);
    
    s = buff.substring(0, buff.indexOf(","));
    buff = buff.substring(buff.indexOf(",")+1);
    r = Integer.parseInt(s);
      
    s = buff.substring(0, buff.indexOf(","));
    buff = buff.substring(buff.indexOf(",")+1);
    g = Integer.parseInt(s);
     
    s = buff.substring(0, buff.indexOf(","));
    buff = buff.substring(buff.indexOf(",")+1);
    b = Integer.parseInt(s);
      
    audio = buff.substring(0, buff.indexOf(")"));
  }else if(buff.indexOf("(cedula") >= 0){//Está reebendo valor de cédula (cedula;2)
    cedula = true;
    valCedula = buff.substring(buff.indexOf(";")+1, buff.indexOf(")"));
    img = valCedula + ".jpg";
    audio = valCedula;
    valCedula += " Reais";
  }else{
    msg = buff;
  }
  
  buff = "";
} 

void executa(String arq){
  try{
      song = minim.loadFile("voz/" + arq + ".mp3");
    }catch(NullPointerException e){}
    delay(500);
    song.play(); 
}
