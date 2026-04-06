using UnityEngine;
using System.Collections;
using System.Collections.Generic;

using UnityEngine.Audio;

public class MainManager : MonoBehaviour
{
    public static MainManager Instance;

    //Weapon Fields
    List<gunStats> gunList = new List<gunStats>();
    int selectedGunPOS;

    //Setting Fields
    float sensitivity = -1;
    float musicVolume = -1;
    float SFXVolume = -1;

    //getters
    public List<gunStats> GetGunList() {return gunList; }
    public int GetSelectedGunPOS() { return selectedGunPOS; }
    public float GetSensitivity() { return sensitivity; }
    //public AudioMixer GetMusicMixer() { return musicMixer; }
    //public AudioMixer GetSFXMixer() { return SFXMixer; }
    public float GetMusicsVolume() { return musicVolume; }
    public float GetSFXVolume() { return SFXVolume; }

    //setters
    public void SetMusicVolume(float volume) { musicVolume = volume; }
    public void SetSFXVolume(float volume) { SFXVolume = volume; }
    //public void SetMusicMixer(AudioMixer mixer) { musicMixer = mixer; }
    //public void SetSFXMixer(AudioMixer mixer) { SFXMixer = mixer; }
    public void SetSensitivity(float sensitivityPassedIn) 
    { 
        sensitivity = sensitivityPassedIn; 
    } 
    public void SetGunList(List<gunStats> gunListPassedIn)
    {
        if(gunList == null)
        {
            gunList = gunListPassedIn;
        }
        else
        {
            gunList = gunListPassedIn;
        }
    }
    public  void SetSelectedGunPos(int selectedGunPOSPassedIn)
    {
        selectedGunPOS = selectedGunPOSPassedIn;
    }

    private void Awake()
    {
        if (Instance != null)
        {
            Destroy(gameObject);
            return;
        }
        Instance = this;
        DontDestroyOnLoad(gameObject);
    }
}
