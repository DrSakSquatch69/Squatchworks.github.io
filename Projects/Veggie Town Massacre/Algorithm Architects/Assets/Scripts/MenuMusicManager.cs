using System.Collections;
using System.Collections.Generic;
using System.Transactions;
using Unity.VisualScripting;
using UnityEngine;
using System;
using Random = UnityEngine.Random;

public class MenuMusicManager : MonoBehaviour
{
    //public delegate void SoundFinishedHandler();
    //public event SoundFinishedHandler OnResumeFinished;

    public static MenuMusicManager Instance;
    [SerializeField] AudioSource Ambient;

    [SerializeField] public AudioSource PlayButtonClick;
    [SerializeField] public AudioClip playButtonClip;
    private Action OnPlayButtonFinished;

    [SerializeField] public AudioSource QuitButtonClick;
    [SerializeField] List<AudioClip> QuitQuips;
    public AudioClip curQuitQuip;
    private Action OnQuitFinished;

    [SerializeField] public AudioSource LoseMenuUp;
    [SerializeField] List<AudioClip> LoseMenuUpClips;
    public AudioClip curLoseUpClip;

    [SerializeField] public AudioSource NextLevelButton;
    [SerializeField] List<AudioClip> NextLevelClips;
    public AudioClip curNextLevelClip;
    private Action OnNxtLvlFinished;

    [SerializeField] public AudioSource OnPauseButton;
    [SerializeField] List<AudioClip> OnPauseQuips;
    public AudioClip curPauseQuip;

    [SerializeField] public AudioSource ResumeButtons;
    [SerializeField] List<AudioClip> ResumeClips;
    public AudioClip curResumeClip;
    public AudioClip getCurResumeClip() { return curResumeClip; }
    private Action onResumeFinished;

    [SerializeField] public AudioSource FinalWin;
    [SerializeField] AudioClip FinalWinClip;

    [SerializeField] public AudioSource LoseRestart;
    [SerializeField] public AudioClip LoseRestartClip;
    private Action OnLoseRestartFinished;

    [SerializeField] public AudioSource NextLevMenuUp;
    [SerializeField] AudioClip NextLevMenUpClip;

    [SerializeField] public AudioSource SettingsUp;
    [SerializeField] AudioClip SettingClip;

    [SerializeField] public AudioSource WinMMBtn;
    [SerializeField] public AudioClip WinMMbtnClip;
    private Action OnWinMMFinished;

    [SerializeField] public AudioSource WinRestart;
    [SerializeField] public AudioClip WinRestartClip;
    private Action onWinRestartFinished;
    void Start()
    {
        if (Instance == null)
        {
            Instance = this; // makes sure we only have one in the scene

        }
        else
        {
            Destroy(this);
        }
        Ambient.Play();
    }
    public void PlayAmbient()
    {
        if (!Ambient.isPlaying) { Ambient.Play(); }
    }
    // Update is called once per frame
    void Update()
    {

    }
    public void StopAmbientSound()
    {
        Ambient.Stop();
    }



    public void PlaySettings()
    {
        if (OnPauseButton.isPlaying || ResumeButtons.isPlaying || QuitButtonClick.isPlaying || NextLevelButton.isPlaying || LoseMenuUp.isPlaying)
        {
            OnPauseButton.Stop();
            ResumeButtons.Stop();
            QuitButtonClick.Stop();
            NextLevelButton.Stop();
            LoseMenuUp.Stop();
        }
        StopAmbientSound();
        SettingsUp.Play();
        PlayAmbient(); 
    }

    public void PlayNextLevelMenUp()
    {
        if (!NextLevMenuUp.isPlaying) { NextLevMenuUp.Play(); }
    }


    public void PlayFinWin()
    {
        if (!FinalWin.isPlaying) { FinalWin.Play(); }
    }


    public void PlayPauseUp()
    {
        int randomIndex = Random.Range(0, OnPauseQuips.Count - 1);
        OnPauseButton.clip = OnPauseQuips[randomIndex];
        curPauseQuip = OnPauseQuips[randomIndex];

        if (SettingsUp.isPlaying || ResumeButtons.isPlaying || QuitButtonClick.isPlaying || NextLevelButton.isPlaying || LoseMenuUp.isPlaying)
        {
            NextLevelButton.Stop();
            ResumeButtons.Stop();
            QuitButtonClick.Stop();
            SettingsUp.Stop();
            LoseMenuUp.Stop();
        }

        OnPauseButton.Play();

    }


    public void PlayLoseUp()
    {
        int randomIndex = Random.Range(0, LoseMenuUpClips.Count - 1);
        LoseMenuUp.clip = LoseMenuUpClips[randomIndex];
        curLoseUpClip = LoseMenuUpClips[randomIndex];

        if (OnPauseButton.isPlaying || ResumeButtons.isPlaying || QuitButtonClick.isPlaying || SettingsUp.isPlaying)
        {
            OnPauseButton.Stop();
            ResumeButtons.Stop();
            QuitButtonClick.Stop();
            SettingsUp.Stop();
        }

        LoseMenuUp.Play();
    }
    public IEnumerator PlayWinMainMenu(Action callback)
    {
        OnWinMMFinished = callback;
        StopAmbientSound();
        WinMMBtn.Play();
        while(WinMMBtn.isPlaying) { yield return null; }
        OnWinMMFinished?.Invoke();
    }
    public IEnumerator PlayLoseRestart(Action callback)
    {
        OnLoseRestartFinished = callback;
        StopAmbientSound();
        LoseRestart.Play();
        while(LoseRestart.isPlaying) { yield return null; }
        OnLoseRestartFinished?.Invoke();
    }
    public IEnumerator PlayButtonSound(Action callback)
    {
        OnPlayButtonFinished = callback;
        StopAmbientSound();
        PlayButtonClick.Play();
        while(PlayButtonClick.isPlaying) { yield return null; }
        OnPlayButtonFinished?.Invoke();
    }

    public IEnumerator PlayNextLevel(Action callback)
    {
        OnNxtLvlFinished = callback;
        int randomIndex = Random.Range(0, NextLevelClips.Count - 1);
        NextLevelButton.clip = NextLevelClips[randomIndex];

        if (OnPauseButton.isPlaying || QuitButtonClick.isPlaying || ResumeButtons.isPlaying || SettingsUp.isPlaying)
        {
            OnPauseButton.Stop();
            ResumeButtons.Stop();
            QuitButtonClick.Stop();
            SettingsUp.Stop();
        }
        StopAmbientSound();
        NextLevelButton.Play();
        while(NextLevelButton.isPlaying) { yield return null; }
        OnNxtLvlFinished?.Invoke();
    }
    public IEnumerator QuitButtonSound(Action callback)
    {
        OnQuitFinished = callback;
        int randomIndex = Random.Range(0, QuitQuips.Count - 1);
        QuitButtonClick.clip = QuitQuips[randomIndex];

        if (OnPauseButton.isPlaying || ResumeButtons.isPlaying || SettingsUp.isPlaying || NextLevelButton.isPlaying || LoseMenuUp.isPlaying)
        {
            OnPauseButton.Stop();
            ResumeButtons.Stop();
            SettingsUp.Stop();
            NextLevelButton.Stop();
            LoseMenuUp.Stop();
        }
        StopAmbientSound();
        QuitButtonClick.Play();
        while(QuitButtonClick.isPlaying) { yield return null; }
        OnQuitFinished?.Invoke();
    }
    public IEnumerator PlayWinRestart(Action callback)
    {
        onWinRestartFinished = callback;
        StopAmbientSound();
        WinRestart.Play();
        while(WinRestart.isPlaying) { yield return null; }
        onWinRestartFinished?.Invoke();

    }
    public IEnumerator PlayResume(Action callback)
    {
        onResumeFinished = callback;
        int randomIndex = Random.Range(0, ResumeClips.Count - 1);
        ResumeButtons.clip = ResumeClips[randomIndex];
        curResumeClip = ResumeButtons.clip;

        if (OnPauseButton.isPlaying || SettingsUp.isPlaying || QuitButtonClick.isPlaying || LoseMenuUp.isPlaying)
        {
            OnPauseButton.Stop();
            LoseMenuUp.Stop();
            QuitButtonClick.Stop();
            SettingsUp.Stop();
        }
        StopAmbientSound();
        ResumeButtons.Play();
        while(ResumeButtons.isPlaying) { yield return null; }
        Debug.Log("Yield Finished");
        onResumeFinished?.Invoke();
        //yield return new WaitForSeconds(curResumeClip.length);
        //OnResumeFinished?.Invoke();
        //PlayAmbient();
    }
}
