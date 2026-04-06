using System.Collections;
using System.Collections.Generic;
using UnityEngine;
//commment
public class pickUp : MonoBehaviour
{
    [SerializeField] gunStats gun;
    bool floatUp;

    [SerializeField] bool flipRotation;

    bool inRange;
    bool itemIsPickedUp;
    private void Start()
    {
        gun.ammo = gun.magSize;
        floatUp = true;

        if (flipRotation)
        {
            transform.eulerAngles = new Vector3(-90f, 0f, 0f);
        }

    }

    void Update()
    {
        if (!gameManager.instance.isPaused)
        {

            if (flipRotation)
            {
                transform.Rotate(0, 0, 0.5f);
            }
            else
            {
                transform.Rotate(0, 0.5f, 0);
            }

            if ((floatUp))
            {
                StartCoroutine(floatingUp());
            }
            else if (!floatUp)
            {
                StartCoroutine(floatingDown());
            }
        }

        if (inRange)
        {
            gameManager.instance.playerScript.Interact(gun);
        }

        if (gameManager.instance.playerScript.isInteract && inRange)
        {
            itemIsPickedUp = true;
            gameManager.instance.playerScript.isInteract = false;
            gameManager.instance.playerScript.isInteractable = false;
            gameManager.instance.turnOnOffInteract.SetActive(false);

            if (gameManager.instance.playerScript.getGun)
            {
                gameManager.instance.playerScript.getGunStats(gun);
                gameManager.instance.playerScript.getGun = false;
            }
            Destroy(gameObject);
        }

    }

    IEnumerator floatingUp()
    {
        if (flipRotation)
        {
            transform.Translate(Vector3.forward * 0.2f * Time.deltaTime);
            yield return new WaitForSeconds(2);
            floatUp = false;
        }
        else
        {
            transform.Translate(Vector3.up * 0.2f * Time.deltaTime);
            yield return new WaitForSeconds(2);
            floatUp = false;
        }
    }

    IEnumerator floatingDown()
    {
        if (flipRotation)
        {
            transform.Translate(-(Vector3.forward * 0.2f * Time.deltaTime));
            yield return new WaitForSeconds(2);
            floatUp = true;
        }
        else
        {
            transform.Translate(-(Vector3.up * 0.2f * Time.deltaTime));
            yield return new WaitForSeconds(2);
            floatUp = true;
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player") && !itemIsPickedUp && !gameManager.instance.isPaused)
        {
            gameManager.instance.turnOnOffInteract.SetActive(true);
            gameManager.instance.interact.text = "Press E to Pickup";
            gameManager.instance.playerScript.isInteractable = true;
            inRange = true;
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player") && !itemIsPickedUp)
        {
            gameManager.instance.turnOnOffInteract.SetActive(false);
            gameManager.instance.playerScript.isInteractable = false;
            inRange = false;
        }
    }
}
