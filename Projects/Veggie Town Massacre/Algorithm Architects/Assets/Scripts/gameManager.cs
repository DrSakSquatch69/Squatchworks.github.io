using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using System.Runtime.InteropServices.WindowsRuntime;
using System;
using UnityEngine.Rendering;
using UnityEngine.Rendering.PostProcessing;
using UnityEngine.Rendering.Universal;

public class gameManager : MonoBehaviour
{
    PlayerSoundManager soundManager;
    [SerializeField] MenuMusicManager menuMusicManager;
    public static gameManager instance;                                     //how we will access game manager
    [SerializeField] GameObject menuActive, menuPause, menuSettings, menuWin, menuLose, menuNextLevel, hitMarker, screenFlash, reloading, noAmmo;
    //[SerializeField] GameObject dialogueBox;                                //Set apart so it can be commented out / turned off
    [SerializeField] TMP_Text enemyCountText;
    public GameObject turnOnOffAmmoText, turnOnOffAmmoText2, turnOnOffAmmoText3, turnOnOffInteract;
    public GameObject icon1, icon2, icon3;
    public Image Scroll1, Scroll2, Scroll3;
    public RawImage iconImage1, iconImage2, iconImage3;
    public TMP_Text interact;
    public TMP_Text remainingAmmoText, remainingAmmoText2, remainingAmmoText3;
    [SerializeField] int nextWaveTimer;
    [SerializeField] GameObject toRespawn;
    [SerializeField] int maxEnemiesAtOnce;
    [SerializeField] Transform respawnPoint;
    [SerializeField] int enemyCountForCurrentLevel;
    [SerializeField] Image tomatoSplat;
    [SerializeField] int splatTime;
    [SerializeField] int fadeOutTime;
    public GameObject enemyHpParent;
    public TMP_Text rayText;
    public PlayerController playerScript;
    public CameraController cameraController;
    public void setPlayerScript(PlayerController script) { playerScript = script; }
    public void setCameraScript(CameraController script) { cameraController = script; }

    public Image playerHPBar;

    [SerializeField] private Volume postProcessingVolume;
    private UnityEngine.Rendering.Universal.DepthOfField depthOfFieldEffect;
    //private bool isBlurred;

    [SerializeField] bool isFinalLevel;
    bool inCredits;
    int currWave;
    bool lastWave = true;

    float timeScaleOrig;                                                    // original timeScale
    GameObject player;                                                     //player object so we can access our player through the game manager
    GameObject daikonKing;
    float playerSpeed;
    float originalPlayerSpeed;
    int playerJumpSpeed;
    int originalPlayerJumpSpeed;
    float alpha; //Transparency for tomato

    int enemyCount;
    int activeEnemies;
    // bool hasDialogueRun;                                                     //to keep track of if the dialogue box has run yet

    public bool isPaused;                                                   //variable to store wether we are paused or not
    bool isReloading;
    bool isNoAmmo;
    bool IsButtered;
    bool inSettings;
    bool isOnFire;
    bool isTomatoed;
    bool isCabbaged;
    bool isProtected;

    IEnumerator tomatoTrack;
    AudioSource playerAudioSource;

    int daikonCount;

    //GETTERS
    public bool getIsPaused() { return isPaused; }                         //getter for our is paused bool
    public GameObject getPlayer() { return player; }                        // getter for player to use in DamageReciever Class
    public GameObject getDaikonKing() { return daikonKing; }
    public bool getIsButtered() { return IsButtered; }
    public float getPlayerSpeed() { return playerSpeed; }
    public float getOriginalPlayerSpeed() { return originalPlayerSpeed; }
    public bool getInSettings() { return inSettings; }
    public AudioSource getSound() { return playerAudioSource; }
    public bool getIsOnFire() { return isOnFire; }
    public bool getIsTomatoed() { return isTomatoed; }
    public bool getIsCabbaged() { return isCabbaged; }
    public PlayerSoundManager GetSoundManager() { return soundManager; }
    public int getDaikonCount() { return daikonCount; }
    public MenuMusicManager GetMenuMusicManager() { return menuMusicManager; }
    //SETTERS
    public void setIsPaused(bool paused) { isPaused = paused; }           // setter for is paused bool 
    public void setIsButtered(bool butter) { IsButtered = butter; }
    public void setPlayerSpeed(float speed) { playerSpeed = speed; }
    public void setOriginalPlayerSpeed(float speed) { originalPlayerSpeed = speed; }
    public void setInSettings(bool settings) { inSettings = settings; }
    public void setSound(AudioSource audio) { playerAudioSource = audio; }
    public void setIsOnFire(bool fire) { isOnFire = fire; }
    public void setIsTomatoed(bool tomato) { isTomatoed = tomato; }
    public void setIsCabbaged(bool cabbage) { isCabbaged = cabbage; }
    public void setCurrWave(int wave) { currWave = wave; }
    public void setLastWave(bool isLastWave) { lastWave = isLastWave; }
    public void setDaikonCount(int daikon) { daikonCount = daikon; }

    void Awake()                                                            //awake always happens first  
    {
        if (instance == null)
        {
            instance = this; // makes sure we only have one in the scene
        }
        else
        {
            Destroy(this);
        }

        if (SceneManager.GetActiveScene().buildIndex == 12)
        {
            inCredits = true;
        }

        else
        {
            inCredits = false;
        }

        timeScaleOrig = Time.timeScale;                                     // setting the original time scale to reset after pause
        player = GameObject.FindWithTag("Player");                          //Tracks player's location 
        daikonKing = GameObject.FindWithTag("DaikonKing");
        tomatoSplat.color = new Color(0, 0, 0, 0);

        if (inCredits != true)
        {
            soundManager = player.GetComponent<PlayerSoundManager>();
        }

        else
        {
            //Play credits music
        }


        GameObject[] daikonFind = (GameObject.FindGameObjectsWithTag("Daikon"));
        daikonCount = daikonFind.Length;
        //updateGameGoal(enemyCountForCurrentLevel);                          //Sets the enemy count text to the proper number
        //Waves();                                                            //Spawns in the first wave of enemies
        // if (postProcessingVolume.profile.TryGet(out depthOfFieldEffect))
        //  {
        //     depthOfFieldEffect.active = false;
        // }
    }

    // Update is called once per frame
    void Update()
    {
        //if (hasDialogueRun == false)
        //{
        //    dialogue();
        //}

        if (inCredits != true)
        {
            if (Input.GetButtonDown("Cancel"))
            {
                if (menuActive == null)                                        //if active menu is null we are in game if not null we are in menu
                {
                    statePause();                                               // calling Fn to create the paused state
                    MenuMusicManager.Instance.StopAmbientSound();
                    MenuMusicManager.Instance.PlayPauseUp();
                    menuActive = menuPause;                                 // setting active menu variable
                    menuActive.SetActive(isPaused);                         //setting menu active via our variable
                }

                else if (inSettings) //Checks if the player is in the settings menu when escape is pressed so it can close the settings menu
                {
                    menuActive.SetActive(false);
                    menuActive = menuPause;
                    menuActive.SetActive(true);
                    inSettings = false;
                }

                else if (menuActive == menuPause)                           //if we are in the pause menu
                {
                    stateUnpause();                                         //change game state
                }

            }
        }
    }
    public void statePause()                                            // changes game state to a paused state
    {
        isPaused = !isPaused;                                           // toggles on and off
        Time.timeScale = 0;                                             //sets the game time to zero so nothing can happen while paused
        Cursor.visible = true;                                          //make cursor visible
        Cursor.lockState = CursorLockMode.Confined;                     //confine cursor to game screen
    }

    public void settingsMenuUp() //Method for bringing menu up here so it can get called in ButtonFns
    {
        MenuMusicManager.Instance.PlaySettings();
        menuActive.SetActive(false);
        menuActive = menuSettings;
        menuActive.SetActive(true);
    }

    public void stateUnpause()                                          //changes game state to un paused
    {
        isPaused = !isPaused;                                           // toggles on and off
        Time.timeScale = timeScaleOrig;                                 // sets our time scale to active using our variable we stored orig timescale in 
        Cursor.visible = false;                                         //rendering cursor not visible
        Cursor.lockState = CursorLockMode.Locked;                       //locking the cursor
        menuActive.SetActive(false);                                    //setting the active menu to inactive
        menuActive = null;                                              //and changes our var back to null
        MenuMusicManager.Instance.PlayAmbient();
    }

    public void stateUnpauseMainMenu()                                  //unpauses the game for the main menu
    {

        isPaused = !isPaused;                                           // toggles on and off
        Time.timeScale = timeScaleOrig;                                 // sets our time scale to active using our variable we stored orig timescale in 

        if (inCredits != true)
        {
            menuActive.SetActive(false);                                    //setting the active menu to inactive
        }
        menuActive = null;
    }

    public void updateGameGoal(int amount)
    {
        enemyCount += amount;                                           //updating enemy count
        enemyCountText.text = enemyCount.ToString("F0");                //updating the enemy count text

        if (enemyCount <= 0)
        {

            if (isFinalLevel && lastWave)
            {
                MenuMusicManager.Instance.PlayFinWin();
                statePause();
                menuActive = menuWin;
                menuActive.SetActive(isPaused);
            }
            else if (!isFinalLevel && lastWave)
            {
                MenuMusicManager.Instance.PlayNextLevelMenUp();
                statePause();
                menuActive = menuNextLevel;
                menuActive.SetActive(isPaused);
            }
        }
    }

    public void youLose()
    {
        statePause();
        MenuMusicManager.Instance.PlayLoseUp();
        menuActive = menuLose;
        menuActive.SetActive(true);
    }

    public IEnumerator ActivateDeactivateHitMarker()
    {
        //turns on and off hitmarker
        hitMarker.SetActive(true);
        yield return new WaitForSeconds(.08f);
        hitMarker.SetActive(false);
    }

    public IEnumerator hitFlash()
    {
        screenFlash.SetActive(true);
        yield return new WaitForSeconds(.08f);
        screenFlash.SetActive(false);
    }

    //public void dialogue()
    //{
    //    statePause();
    //    menuActive = dialogueBox;
    //    menuActive.SetActive(isPaused);
    //    hasDialogueRun = true;
    //}

    public void UpdateAmmoCounter(int ammo, int remainingAmmo, int i)
    {
        if (i == 0)
        {
            remainingAmmoText.text = ammo.ToString("F0") + " / " + remainingAmmo.ToString("F0");
        }
        else if (i == 1)
        {
            remainingAmmoText2.text = ammo.ToString("F0") + " / " + remainingAmmo.ToString("F0");
        }
        else if (i == 2)
        {
            remainingAmmoText3.text = ammo.ToString("F0") + " / " + remainingAmmo.ToString("F0");
        }
    }

    public void reloadingOnOff()
    {
        isReloading = !isReloading;
        reloading.SetActive(isReloading);
    }
    public void NoAmmoOnOff()
    {
        isNoAmmo = !isNoAmmo;
        noAmmo.SetActive(isNoAmmo);
    }

    public bool ActiveCheck(int amount) //Checks to see if there are no active enemies left 
    {
        activeEnemies = amount;

        if (activeEnemies == 0)
        {
            return true;
        }

        return false;
    }

    public void Waves() //Responsible for spawning in the correct amount of enemies per wave at a specific location
    {
        if (ActiveCheck(activeEnemies))
        {

            while (activeEnemies < maxEnemiesAtOnce && activeEnemies < enemyCount) //Makes enemies until the max at a time is reached
            {
                //StartCoroutine(GradualSpawning());
                //if (activeEnemies > enemyCountForCurrentLevel)
                //{
                //    return;
                //}
                ++activeEnemies;
                GameObject.Instantiate(toRespawn, respawnPoint.position, transform.rotation);

            }
        }
    }

    public int GetEnemyCountCurrent()
    {
        return enemyCount;
    }

    public void TomatoSplat()
    {
        if (isTomatoed)
        {
            //Gets rid of tomato after player dies
            if (playerScript.HealthPoints <= 0)
            {
                tomatoSplat.color = new Color(0, 0, 0, 0);
                isTomatoed = false;
                return;
            }

            //If an instance of the coroutine is running already stop it so it doesn't interfere with the one about to be called
            if (tomatoTrack != null)
            {

                StopCoroutine(tomatoTrack);
            }

            //Sets up the variable checked above. Allows all instances to be stopped instead of just one instance
            tomatoTrack = FadeOut();
            StartCoroutine(tomatoTrack);
        }

    }

    IEnumerator FadeOut()
    {
        alpha = 1;

        //Brings up tomato to screen
        tomatoSplat.color = new Color(1, 1, 1, 1);

        yield return new WaitForSeconds(splatTime);

        //While tomato is still on screen slowly lower its opacity (alpha value)
        while (alpha >= 0)
        {
            tomatoSplat.color = new Color(1, 1, 1, alpha);
            alpha -= fadeOutTime * Time.deltaTime; //Backup code - alpha -= 0.2f;
            yield return new WaitForSeconds(0.1f);  // Backup code - yield return new WaitForSeconds(fadeOutTime / 5);
        }

        tomatoSplat.color = new Color(1, 1, 1, 0);
        isTomatoed = false;
    }

    public void EnableBlur(float duration)
    {
        if (depthOfFieldEffect == null) return;

        //isBlurred = true;
        depthOfFieldEffect.active = true;
        depthOfFieldEffect.focusDistance.value = 0.1f; // High blur effect
        StartCoroutine(DisableBlurAfterTime(duration));
    }

    private IEnumerator DisableBlurAfterTime(float duration)
    {
        yield return new WaitForSeconds(duration);

        if (depthOfFieldEffect != null)
        {
            depthOfFieldEffect.focusDistance.value = 10f; // Reset to normal
            depthOfFieldEffect.active = false;
            //isBlurred = false;
        }
    }

    public void DisableBlur()
    {
        if (depthOfFieldEffect != null)
        {
            depthOfFieldEffect.focusDistance.value = 10f; // Reset to normal
            depthOfFieldEffect.active = false;
            //isBlurred = false;
        }
    }

    //public IEnumerator GradualSpawning()
    // {
    //     yield return new WaitForSeconds(nextWaveTimer);

    // }
}
