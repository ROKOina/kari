using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CoinBox : MonoBehaviour
{
    [SerializeField]
    private int itemCount = 5;

    [SerializeField]
    private GameObject coinPrefab;

    [SerializeField]
    private Transform appearPoint;

    [SerializeField]
    private float pushPower = 0.1f; //����@����

    [SerializeField]
    private float gravity = 1.0f;   //�d��

    private Vector3 originalPosition;   //�����ʒu

    private Vector3 position;   //���݈ʒu

    private bool isMoving;  //�����Ă����Ԃ�

    private float verticalSpeed;    //�����ړ����x

    [SerializeField]
    private Material emptyBoxMaterial; //�}�e���A��


    // Start is called before the first frame update
    void Start()
    {
        position = originalPosition = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        //�����c�h�ꂷ�鏈��
        if(isMoving)
        {
            verticalSpeed -= gravity * Time.deltaTime;
            position.y += verticalSpeed;
            if(position.y<originalPosition.y)
            {
                position.y = originalPosition.y;
                isMoving = false;
            }
            transform.position = position;
        }
    }

    //���̉����ɓ����������̏���
    private void OnTriggerEnter(Collider other)
    {
        //�v���C���[�ɏՓ˂����ꍇ
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //�R�C�����o������񐔂𐧌�
            if(itemCount>0)
            {
                itemCount--;

                //����ۂ̃}�e���A���ɐ؂�ւ�
                if(itemCount==0)
                {
                    MeshRenderer meshRenderer = GetComponent<MeshRenderer>();
                    meshRenderer.material = emptyBoxMaterial;
                }

                //���̏c�h�ꏈ���J�n
                verticalSpeed = pushPower;
                isMoving = true;

                //�R�C������
                GameObject coin = Instantiate(coinPrefab, appearPoint.position, Quaternion.identity);
                Destroy(coin.gameObject, 0.5f); //������A�w�莞�Ԍ�ɍ폜
                player.AddCoin(1);  //�R�C�����l��

                //���̃R���|�[�l���g��ǉ����A������ɗ͂�ݒ�
                Rigidbody body = coin.AddComponent<Rigidbody>();
                body.velocity = new Vector3(0.0f, 6.0f, 0.0f);
            }
        }
    }
}