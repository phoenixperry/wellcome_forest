using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

public class serialEx : MonoBehaviour
{
    public SerialController serialController;
    int timeBetweenUpdates = 250;
    System.DateTime lastUpdate = System.DateTime.Now;
    System.DateTime currentTime = System.DateTime.Now;

    // Start is called before the first frame update
    void Start()
    {

        serialController = GameObject.Find("SerialController").GetComponent<SerialController>();

    }

    // Update is called once per frame
    void Update()
    {
        currentTime = System.DateTime.Now;
        if ((currentTime - lastUpdate).Milliseconds > timeBetweenUpdates)
        {
            Debug.Log("Updating Slaves to: {111211311411511611711811911A11B11}\n");
            serialController.SendSerialMessage("{111211311411511611711811911A11B11}\n");
            lastUpdate = currentTime;
        }
        
    }

    // Invoked when a line of data is received from the serial device.
    // This is from the example code for the package Ardity
    void OnMessageArrived(string msg)
    {
        Debug.Log("Arrived: " + msg);
    }

    // Invoked when a connect/disconnect event occurs. The parameter 'success' will be 
    // 'true' upon connection, and false upon disconnection or failure to connect
    // This is from the example code for the package Ardity
    void OnConnectionEvent(bool success)
    {
        Debug.Log(success ? "Device connected" : "Device disconnected");
    }
}
