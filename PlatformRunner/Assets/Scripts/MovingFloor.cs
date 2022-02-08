using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovingFloor : MonoBehaviour
{
    [SerializeField]
    private Vector3 move = new Vector3(-2.0f, 0.0f, 0.0f);  //�ړ���

    [SerializeField]
    private float time = 1.0f;  //���̈ʒu����ړI�̈ʒu���������鎞��

    private Vector3 originalPosition;   //���̈ʒu

    private float theta;

    // Start is called before the first frame update
    void Start()
    {
        originalPosition = transform.position;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        //�w�莞�ԂŃƂ�1������v�Z
        if(time!=0.0f)
        {
            theta += Mathf.PI * 2.0f * (1.0f / time) * Time.deltaTime;
        }
        //�T�C���J�[�u�𗘗p����-1.0�`1.0�̊Ԃ��擾����
        float c = Mathf.Sin(theta);
        //-1.0�`1.0�̒l��0.0�`1.0�̒l�ɕύX����
        c = (1.0f - c) * 0.5f;
        //���̈ʒu����ړI�̈ʒu�܂ł���������ʒu���Z�o����
        Vector3 position = Vector3.Lerp(originalPosition, originalPosition + move, c);
        transform.position = position;
    }
}
