using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class PlayerSoundManager : MonoBehaviour
{//
    public static PlayerSoundManager Instance;

    [SerializeField] AudioSource walking;
    [SerializeField] AudioSource running;
    [SerializeField] AudioSource crouched;
    [SerializeField] AudioSource wallRun;
    [SerializeField] AudioSource doubleJump;
    [SerializeField] AudioSource landing;
    [SerializeField] AudioSource takingDamage;
    [SerializeField] AudioSource Sliding;
    [SerializeField] AudioSource Bounce;
    [SerializeField] AudioSource BulletDamage;
    [SerializeField] AudioSource MeleeDamage;
    [SerializeField] AudioSource ChaserDamage;
    [SerializeField] AudioSource ButterDamage;
    [SerializeField] AudioSource StationaryDamage;
    [SerializeField] AudioSource deathSound;
    [SerializeField] AudioSource winningSound;
    [SerializeField] AudioSource finalWiningSound;
    [SerializeField] AudioSource flashlightOn;
    [SerializeField] AudioSource flashlightOff;
    [SerializeField] AudioSource melee;
    [SerializeField] AudioSource FirstJump;
    [SerializeField] AudioSource ReloadSounding;
    [SerializeField] AudioSource shootSource;
    [SerializeField] AudioSource explosion;

    [SerializeField] float runPitch;
    [SerializeField] float walkPitch;
    [SerializeField] float crouchPitch;
    [SerializeField] float crouchVolume;

    public bool isWalkingPlaying;
    public bool isRunningPlaying;
    public bool isCrouchedPlaying;

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
    }
    public bool walkingPlaying()
    {
        if (walking.isPlaying) 
            isWalkingPlaying = true;
        else 
            isWalkingPlaying = false;

        return isWalkingPlaying;
    }

    public bool runningPlaying()
    {
        if(running.isPlaying)
            isRunningPlaying = true;
        else
            isRunningPlaying = false;

        return isRunningPlaying;
    }

    public bool crouchedPlaying()
    {
        if (crouched.isPlaying)
            isCrouchedPlaying = true;
        else
            isCrouchedPlaying = false;

        return isCrouchedPlaying;
    }

    private bool hasLanded = false;

    private float audioOrigVolume;
    private float audioOrigPitch;
    private void Awake()
    {
        if(Instance == null)
        {
            Instance = this;
        }
        else
        {
            Destroy(gameObject);
        }
    }
    public void PlayWalking()
    {
        if(!walking.isPlaying)
        {
            walking.Play();
        }
    }
    public void StopWalking()
    {
        if(walking.isPlaying)
        {
            walking.Stop();
        }
    }

    public void PlayCrouch()
    {
        if(crouched.isPlaying)
        {
            crouched.Play();
        }
    }

    public void StopCrouch()
    {
        crouched.Stop();
    }
    public void PlayRun()
    {
        if(running.isPlaying)
        {
            running.Play();
        }
    }

    public void StopRun()
    {
        running.Stop();
    }
    public void PlayWallRun()
    {
        if(!wallRun.isPlaying)
        {
            wallRun.Play();
        }
    }

    public void stopWallRun()
    {
        if(wallRun.isPlaying)
        {
            wallRun.Stop();
        }
    }

    public void PlayDoubleJump()
    {
        if(!doubleJump.isPlaying)
        {
            doubleJump.Play();
        }
    }

    public void PlayLanding(bool isGrounded)
    {
        if (isGrounded&& !hasLanded)
        {
            landing.Play();
            hasLanded = true;
        }
        else if (!isGrounded)
        {
            hasLanded= false;
        }
    }

    public void StopLanding()
    {
        landing.Stop();
        hasLanded = false;
    }

    public void PlayTakingDamage()
    {
        if(!takingDamage.isPlaying)
        {
            takingDamage.Play();
        }    
    }
    public void playSliding()
    {
        if(!Sliding.isPlaying)
        {
            Sliding.Play();
        }
    }
    public void stopSliding()
    {
        if(Sliding.isPlaying)
        {
            Sliding.Stop();
        }
    }

    public void PlayBounce()
    {
        if (!Bounce.isPlaying)
        {
            Bounce.Play();
        }
    }    

    public void PlayBulletDMG()
    {
        if(!BulletDamage.isPlaying)
        {
            BulletDamage.Play();
        }
    }

    public void PlayMeleeDMG()
    {
        if(!MeleeDamage.isPlaying)
        {
            MeleeDamage.Play();
        }
    }

    public void PlayChaserDMG()
    {
        if(!ChaserDamage.isPlaying)
        {
            ChaserDamage.Play();
        }
    }

    public void PlayButterDMG()
    {
        if (!ButterDamage.isPlaying)
        {
            ButterDamage.Play();
        }
    }

    public void PlayStationaryDMG()
    {
        if(!StationaryDamage.isPlaying)
        {
            StationaryDamage.Play();
        }
    }

    public void StopStationaryDMG()
    {
        if(!StationaryDamage.isPlaying)
        {
        StationaryDamage.Stop();
        }
    }

    public void PlayDeathSound()
    {
        if(!deathSound.isPlaying)
        {
            deathSound.Play();
        }
    }
    public void PlayWinningSound()
    {
        if(!winningSound.isPlaying)
        {
            winningSound.Play();
        }
    }
    public void PlayFinalWinSound()
    {
        if(!finalWiningSound.isPlaying)
        {
            finalWiningSound.Play();
        }
    }

    public void PlayFlashlightOn()
    {
        if(!flashlightOn.isPlaying)
        {
            flashlightOn.Play();
        }
    }

    public void PlayFlashlightOff()
    {
        if (!flashlightOff.isPlaying)
        {
            flashlightOff.Play();
        }   
    }

    public void PlayMelee()
    {
        if (!melee.isPlaying)
        {
            melee.Play();
        }
    }

    public void PlayFirstJump()
    {
        if(!FirstJump.isPlaying)
        {
            FirstJump.Play();
        }
    }
    
    public void PlayReload()
    {
        if (!ReloadSounding.isPlaying)
        {
            ReloadSounding.Play();
        }
    }

    public void playShootSound(AudioClip clip)
    {
        if(clip != null)
        {

        shootSource.PlayOneShot(clip);
        }
    }

    public void playExplosion(AudioClip clip)
    {
        if (clip != null)
        {
            explosion.PlayOneShot(clip);
        }
    }
}
