using System.Collections;
using System.Collections.Generic;
using UnityEngine;
//
public class CameraController : MonoBehaviour
{
    [SerializeField] float sens;
    [SerializeField] int lockVertMin, lockVertMax;
    [SerializeField] bool invertY;
    [SerializeField] Transform playerCapsule;
    [SerializeField] GameObject cameraShake;

    Vector3 originalPos;

    public float mouseY;
    public float mouseX;

    public float rotX;
    bool isShaking;

    // Start is called before the first frame update
    void Start()
    {
        //Locks the cursor 
        Cursor.visible = false;
        Cursor.lockState = CursorLockMode.Locked;
        gameManager.instance.setCameraScript(this);
        
        originalPos = cameraShake.transform.localPosition;
    }

    // Update is called once per frame
    void Update()
    {
        sens = MainManager.Instance.GetSensitivity(); //Gets the sens set from the settings menu

        // get input
        mouseY = Input.GetAxis("Mouse Y") * sens * Time.deltaTime;
        mouseX = Input.GetAxis("Mouse X") * sens * Time.deltaTime;

        // invert Y camera
        if (!invertY)
            rotX -= mouseY;
        else
            rotX += mouseX;

        // clamp the rotX on the x-axis
        rotX = Mathf.Clamp(rotX, lockVertMin, lockVertMax);

        // rotate the camera on the x-axis
        transform.localRotation = Quaternion.Euler(rotX, 0, 0);

        // rotate the player on the y-axis
        playerCapsule.Rotate(Vector3.up * mouseX);
    }

    public void startShake(float time, float intensity)
    {
        if (!isShaking) 
        {
            StartCoroutine(shake(time, intensity));
        }
    }

    IEnumerator shake(float time, float intensity)
    {

        originalPos = cameraShake.transform.localPosition;
        isShaking = true;

        while (time > 0 && !gameManager.instance.isPaused)
        {
            float x = Random.Range(-1f, 1f) * intensity;
            float y = Random.Range(-1f, 1f) * intensity;

            Vector3 shakePos = new Vector3(x, y, originalPos.z);

            cameraShake.transform.localPosition = originalPos + shakePos;


            time -= Time.deltaTime;

            yield return null;
        }

        cameraShake.transform.localPosition = originalPos;
        isShaking = false;
    }
}

//override