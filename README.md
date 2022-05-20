Computer Vision Target Shooting Robot
=====================================

Objective:
----------
"The design project is to develop a robot that competes in an [arena](https://cdn.discordapp.com/attachments/817281300821049352/977007824553009233/unknown.png). The robot begins in a start box, and when driving forward breaks an IR beam to start the timer. The robot must then strike 4 contact plates and continue on to the finish box where the timer is stopped. The arena must be completed in two different modes, 1) teleoperation where the user drives via commands and images over TCP/IP and 2) autonomous mode where the robot solves the arena without human intervention."

Our Solution:
-------------
We decided to base our design around two [NEMA 17](https://www.amazon.ca/gp/product/B09N3NRTW9/ref=ppx_yo_dt_b_asin_title_o06_s01?ie=UTF8&psc=1) stepper motors using respective [L298N](https://www.amazon.ca/gp/product/B00XAGRQTO/ref=ppx_yo_dt_b_asin_title_o07_s00?ie=UTF8&psc=1) driver circuit boards and a [Teensy 4.0](https://www.amazon.ca/Teensy-4-0-with-Pins/dp/B08259KDHY/ref=sr_1_1?crid=2YYR6ATL3VK35&keywords=teensy+4.0&qid=1653007497&sprefix=teensy+4+0%2Caps%2C522&sr=8-1) to interface the motors to the [Raspbery Pi 4b](https://www.amazon.ca/s?k=rasberry+pi+4+b&crid=1HV9MO5X30TG&sprefix=rasb%2Caps%2C238&ref=nb_sb_ss_ts-doa-p_1_4). We would use two [ultrasonic sensors](https://www.sparkfun.com/products/15569) for positional alignment, a [servo](https://www.amazon.ca/gp/product/B07VJG5QTJ/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1) for turret aiming, a [servo](https://www.amazon.ca/gp/product/B07KR24YX8/ref=ppx_yo_dt_b_asin_title_o03_s00?ie=UTF8&psc=1) for firing at the targets, and finally a [pi camera](https://www.amazon.ca/Keyestudio-Camera-Module-5MP-Raspberry/dp/B073RCXGQS/ref=sr_1_5?crid=10T4SHGENUO6L&keywords=pi%2Bcamera&qid=1653007276&sprefix=p%2Caps%2C299&sr=8-5&th=1). We decided to split the main functionality of the robot into two parts: Driving (Teensy) and Detection (Raspberry Pi). There are two main reasons for this decision, first we wanted to unload as much possible processing from the raspberry pi so that our computer vision algorithm could run faster, and second it allowed us to completely isolate our workloads into two seperate entities, and thus we could work asychronously. 

For Auto mode our algorithm was based around a state machine, each state being defined as a portion of the arena. The Teensy would drive the motors to the first location, wait for feedback from the Pi on when the target was successfully hit, then move to the second target, and so on. The Pi would grab the state of each target from a server hosted by the arena over TCP/IP, and pass that information onto the Teensy via UART. 

The method used to aim the cannon was to use the pi camera to scan for ArUCO QR codes, using that information encoded into screenspace, determine whether the target was on the left or right of the centre of the camera. If the ArUCO was in the centre, it would fire the cannon.

In manual driving mode, the user would input keypresses into a windows client on a laptop that communicated with the Pi over TCP/IP. The client additionally fetched a camera feed from the arena, and a camera feed from the Pi to enable the user to fully drive the unit without line of sight to the arena.

Challenges:
-----------
> First iteration of steppers not powerful enough
> 
> PCB Short Circuit
> 
> Power delivery to driver circuit inadequate 
> 
> UART Buffer Issues

Conclusion:
-----------
Lorem Ipsum

Media:
------
![PCB pic](https://lh3.googleusercontent.com/pw/AM-JKLWlMkTerXQYZCwnm_JCvEM7bRIK7TNIcuGkJb2xONFJYlhhucYpDF1-2FL8HiNiAXT6UoejXoz83wnhhLQjHCdD21mIn-QfXMPixGpwxQDp3eeY7PyStENFx31qzlnfrk8oDOJMUpbbsOpOdAF-mMSd=w858-h646-no?authuser=0)
![robot pic 1](https://lh3.googleusercontent.com/pw/AM-JKLX-HMG5jSNgOfybfn1LnD3AL1hMeZ6PdI3bx_U9cVgEtDPb-oQ1MouzEkV2NUGQHuQqXV6Yj9QpyreNjHgiuflL3Aakp7C4lE_I8jKbo2svmMFHf5jvuGZ4H5ESCRAvgXATQSAfH5Y3Dq5gNBLrMiu7=w858-h646-no?authuser=0)
![servo pic](https://lh3.googleusercontent.com/pw/AM-JKLWsN-Wi_gM3c9h_fK201YREAuN78n5Y5iKu6k-gLSe-XNzWk9W0YX6vNvxRjfsnorRv-ScYEzCaVc7AInKIqk7B_rwTEjGl0MUgEss2gKe8Vo4hM71FeO5rMvIYwmO7cd_JEeP6YKGCCOAMhHdBO9qx=w487-h646-no?authuser=0)

[More Robot](https://photos.app.goo.gl/skK17sW5vWhY1REk8)

Credits:
-------
**Kurt Querengesser**

Author of PI code and Lead Parts Designer
[![Kurt Querengesser](https://media-exp1.licdn.com/dms/image/C5603AQGrgBTkykBlKQ/profile-displayphoto-shrink_200_200/0/1631942127837?e=1658361600&v=beta&t=vpEAgsTLOk_cjnBG9KAYYulb9IDrupyVI58InTzyYOE)](https://www.linkedin.com/in/kurt-querengesser/)



**Michael Andrews**

Author of Microcontroller Code and Lead PCB Designer
[![Michael Andrews](https://media-exp1.licdn.com/dms/image/C5603AQGyt1dAC75fPQ/profile-displayphoto-shrink_800_800/0/1609993278505?e=1658361600&v=beta&t=rdH3prSpEyX7Fl4-O2noVDR5wyUUb0KhQuewCrafTK8)](https://www.linkedin.com/in/michael-andrews-b0ab8217a/)

