# ESP32-Solcast-API
Interface for the Solcast Solar Forcast API implemented on ESP32

This is a simple sketch to retrieve the solar forcast for tomorrow from https://toolkit.solcast.com.au
using their API.
You'll need to create a free account which will give you a URL specific to your Solar Installation
and an API Key.  Just fill in the details in the sketch - and away you go!

This is part of my plan to automate the charging of my batteries based on the price of electricity
on Octopus Agile, the solar forcast and data retrieved from a Solis Hybrid Inverter.
I'll upload the individual interfaces as separate sketches, which you can mix up to suit your requirements.
Once I have a working automation - I'll upload that too!

You are probably thinking "Why is he not using Home Automation - what an idiot!" ;-)
Well, I am (as well)!  However, I'm hoping my panels will last 25 years or so.  I'll bet HA and the cloud service for my
inverter won't exist in 25 years.  Writing it on a simpler platform without reliance on 3rd parties gives it a better chance
of continuing to work long term.  If, say the solar forcast changes, it's only one bit I need to re-write.

I hope the code is useful - please use and abuse as you desire!
