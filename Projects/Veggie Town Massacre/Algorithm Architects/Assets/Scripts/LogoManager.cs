using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
 
//
public class LogoManager : MonoBehaviour
{
    public float delay = 25.0f; // time to show the logo
    public float fadeDuration = 2.0f; // Durration for fade in/out
    public SpriteRenderer logoSprite; // reference to the logo
    [SerializeField] AudioSource AudioSource;

    // Start is called before the first frame update
    void Start()
    {
        AudioSource.Play();
        //Invoke("LoadNextScene", delay);
        StartCoroutine(FadeInOut());
    }

    IEnumerator FadeInOut()
    {
        //Step 1: fade in
        yield return StartCoroutine(Fade(0f, 1f, fadeDuration));

        //Step 2: allow time for full logo
        yield return new WaitForSeconds(delay - 2 * fadeDuration);

        //Step 3: fade out
        yield return StartCoroutine(Fade(0f, 1f, fadeDuration));

        //Step 4: load next scene
        SceneManager.LoadScene("Main Menu");

    }

    IEnumerator Fade(float startAlpha, float endAlpha, float duration)
    {
        float elapsedTime = 0f;
        Color color = logoSprite.color;

        //Fade over given duration
        while (elapsedTime < duration)
        {

            elapsedTime += Time.deltaTime;
            float alpha = Mathf.Lerp(startAlpha, endAlpha, elapsedTime / duration);
            logoSprite.color = new Color(color.r, color.g, color.b, alpha);
            yield return null; //Wait until next frame

        }
        //Ensure final alpha is set
        logoSprite.color = new Color(color.r, color.g, color.b, endAlpha);
    }
}
