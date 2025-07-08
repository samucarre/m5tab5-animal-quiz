# Juego educativo con batería, aciertos, fallos.
import M5
from M5 import *
import time
import random
import sys
from machine import Pin, PWM

M5.begin()

# Configuración de pantalla
lcd = M5.Lcd
lcd.setRotation(0)  # Vertical: 540x960
lcd.clear()
lcd.setTextColor(0x000000)
lcd.setTextSize(2)

# Lista de animales
animal_list = [
    "Bear.png", "Beaver.png", "Cat.png", "Chick.png", "Chicken.png", "Cow.png", "Crocodile.png",
    "Deer.png", "Dog.png", "Dolphin.png", "Donkey.png", "Duck.png", "Eagle.png", "Elephant.png",
    "Flamingo.png", "Fox.png", "Frog.png", "Giraffe.png", "Goat.png", "Gorilla.png", "Hamster.png",
    "Hedgehog.png", "Hippo.png", "Horse.png", "Lion.png", "Monkey.png", "Mouse.png", "Octopus.png",
    "Owl.png", "Panda.png", "Parrot.png", "Penguin.png", "Piglet.png", "Polar Bear.png", "Rabbit.png",
    "Raccoon.png", "Rooster.png", "Shark.png", "Sheep.png", "Sloth.png", "Snail.png", "Squirrel.png",
    "Tiger.png", "Turtle.png", "Whale.png", "Wolf.png", "Zebra.png"
]

# Configuraciones
IMG_W = 350
IMG_H = 250
IMG_X = 200
IMG_Y_START = 200
IMG_Y_GAP = 320
positions = [
    (IMG_X, IMG_Y_START),
    (IMG_X, IMG_Y_START + IMG_Y_GAP),
    (IMG_X, IMG_Y_START + 2 * IMG_Y_GAP)
]

correct_animal = ""
options = []
last_touch = (-1, -1)
last_battery_check = time.ticks_ms()
score = 0
fails = 0

# Funciones auxiliares
def shuffle_list(lst):
    for i in range(len(lst) - 1, 0, -1):
        j = random.randint(0, i)
        lst[i], lst[j] = lst[j], lst[i]

def dentro_imagen(x, y, bx, by):
    return bx <= x <= bx + IMG_W and by <= y <= by + IMG_H

def mostrar_bateria():
    try:
        battery = M5.Power.getBatteryLevel()
        if 0 <= battery <= 100:
            lcd.setTextSize(3)
            lcd.setTextColor(0x000000)
            lcd.setCursor(10, 10)
            lcd.print("bat: {}%".format(battery))
        else:
            lcd.setTextSize(3)
            lcd.setTextColor(0x000000)
            lcd.setCursor(10, 10)
            lcd.print("bat: N/A")
    except Exception as e:
        print("Error batería:", e)

def mostrar_puntos():
    lcd.setTextSize(3)
    lcd.setTextColor(0x000000)
    lcd.setCursor(1, 50)
    lcd.print("Score {}".format(score))
    lcd.setCursor(1, 90)
    lcd.print("Fails {}".format(fails))

def mostrar_splash():
    try:
        lcd.drawPng("/flash/background.png", 0, 0)
    except Exception as e:
        print("Error fondo splash:", e)
        lcd.clear()
    try:
        lcd.drawPng("/flash/splash.png", 0, 0)
    except Exception as e:
        print("Error splash:", e)
        lcd.setCursor(150, 200)
        lcd.setTextSize(12)
        lcd.print("Animal Quiz!")
    time.sleep(3)

def nuevo_juego():
    global correct_animal, options
    lcd.clear()
    try:
        lcd.drawPng("/flash/background.png", 0, 0)
    except Exception as e:
        print("Error fondo juego:", e)
    mostrar_bateria()
    mostrar_puntos()
    lcd.setCursor(220, 30)
    lcd.setTextSize(5)
    lcd.setTextColor(0x000000)
    lcd.print("Which one is...")

    correct_animal = random.choice(animal_list)
    lcd.setCursor(220, 100)
    lcd.setTextSize(7)
    lcd.print(correct_animal.split(".")[0])

    opciones_temp = [a for a in animal_list if a != correct_animal]
    shuffle_list(opciones_temp)
    opciones = opciones_temp[:2] + [correct_animal]
    shuffle_list(opciones)
    options.clear()
    options.extend(opciones)

    for i in range(3):
        x, y = positions[i]
        try:
            lcd.drawPng("/flash/" + options[i], x, y)
        except Exception as e:
            print("Error imagen:", options[i], ":", e)

# Inicialización
M5.update()
last_touch = (M5.Touch.getX(), M5.Touch.getY())
mostrar_splash()
nuevo_juego()

# Bucle principal
while True:
    M5.update()
    x = M5.Touch.getX()
    y = M5.Touch.getY()
    if (x, y) != last_touch:
        last_touch = (x, y)
        if 460 <= x <= 540 and 925 <= y <= 955:
            pass  # Apagar dispositivo (puedes implementar esta parte si quieres)
        for i in range(3):
            bx, by = positions[i]
            if dentro_imagen(x, y, bx, by):
                lcd.setTextSize(4)
                lcd.setCursor(240, 170)
                if options[i] == correct_animal:
                    score += 1
                    lcd.setTextColor(0x007700)  # Verde sin fondo
                    lcd.print("Correct!")
                    time.sleep(1.5)
                    nuevo_juego()
                else:
                    fails += 1
                    lcd.setTextColor(0x770000)  # Rojo sin fondo
                    lcd.print("Try again")
                    mostrar_puntos()
                break
    if time.ticks_diff(time.ticks_ms(), last_battery_check) > 10000:
        last_battery_check = time.ticks_ms()
        mostrar_bateria()
    time.sleep(0.1)