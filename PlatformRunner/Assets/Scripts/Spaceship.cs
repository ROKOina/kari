using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Spaceship : MonoBehaviour
{
    [SerializeField]
    private float force = 1.0f;

    private float timer;

    private Rigidbody body;

    // Start is called before the first frame update
    void Start()
    {
        body = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        //�㏸�J�n�����莞�Ԍ�ɃS�[��
        if(timer>0.0f)
        {
            timer -= Time.deltaTime;
            if(timer<0.0f)
            {
                Goal();
            }
        }
    }

    //�Œ莞�Ԗ��X�V����
    private void FixedUpdate()
    {
        if (timer <= 0.0f) return;

        //�㏸����͂�������
        body.AddForce(0, force * Time.deltaTime, 0);
    }

    //�Փ˔��菈��
    private void OnTriggerEnter(Collider other)
    {
        //�v���C���[������������S�[�����o�J�n
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //Goal();
            //�v���C���[���\��
            player.gameObject.SetActive(false);
            //�㏸���鎞�Ԃ�ݒ�i�㏸�J�n�j
            timer = 5.0f;
        }
    }

    //�S�[������
    private void Goal()
    {
        //���݂̃V�[�����ă��[�h
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
    }
}
