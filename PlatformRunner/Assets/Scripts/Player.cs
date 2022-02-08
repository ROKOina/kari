using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

[RequireComponent(typeof(CharacterController))]
[RequireComponent(typeof(Animator))]
public class Player : MonoBehaviour
{
    [SerializeField]
    private float maxMoveSpeed = 5.0f;  //�ő�ړ����x

    private CharacterController controller;

    private float horizontalSpeed;  //�����ړ����x

    [SerializeField]
    private float maxFallSpeed = 20.0f; //�ő嗎�����x

    [SerializeField]
    private float gravity = 60.0f;  //�d��

    private float verticalSpeed;    //�����ړ����x

    [SerializeField]
    private float jumpSpeed = 20.0f;    //�W�����v���x

    [SerializeField]
    private float acceleration = 10.0f; //�����x

    [SerializeField]
    private float friction = 10.0f;  //���C�W��

    [SerializeField]
    private float airControl = 0.3f;    //�󒆓��͑��쎞�̕␳�l

    private Animator animator;

    [SerializeField]
    private Text coinText;

    private int coinCount;  //�l���R�C������

    [SerializeField]
    private Transform spawnPoint;

    [SerializeField]
    private Text lifeText;

    [SerializeField]
    private int life = 5;   //���C�t��

    [SerializeField]
    private float pushPower = 1.0f;    //������

    private Transform activeFloor;  //����ł��鏰

    private Vector3 activeLocalFloorPoint;  //������ɂ����Ƃ��̑��Έʒu

    private Vector3 activeGlobalFloorPoint; //���E����ɍl������Έʒu

    private int airFrame;   //�󒆂ɂ��鎞�ԁi�t���[���j

    // Start is called before the first frame update
    void Start()
    {
        controller = GetComponent<CharacterController>();
        animator = GetComponent<Animator>();
        Spawn();
        //UI�\���X�V
        UpdateCoinText();
        UpdateLifeText();
    }

    // Update is called once per frame
    void Update()
    {
        ////�����������͏��擾
        //float horizontal = Input.GetAxisRaw("Horizontal");

        ////�ړ�����
        //Vector3 move;
        //move.x = horizontal * maxMoveSpeed;
        //move.y = 0.0f;
        //move.z = 0.0f;
        //controller.Move(move * Time.deltaTime);

        //�d�͏���
        UpdateGravity();

        //�W�����v����
        UpdateJump();

        //�i�s�����X�V����
        UpdateDirection();

        //�ړ��X�V����
        UpdateMovement();
    }

    //�Œ莞�Ԗ��X�V����
    private void FixedUpdate()
    {
        //�O�̂���Z���ʒu�������Ȃ��悤�ɌŒ�
        transform.position = new Vector3(transform.position.x, transform.position.y, 0);
    }

    //�i�s�����X�V����
    private void UpdateDirection()
    {
        //�����������͏��擾
        float horizontal = Input.GetAxisRaw("Horizontal");

        //��������
        float power = Mathf.Abs(horizontal);
        if(power>0.1f)
        {
            //���������͏�񂩂�i�s������ݒ�
            Vector3 direction = new Vector3(horizontal, 0, 0);

            //�i�s�����������悤�ɉ�]�ݒ�
            transform.rotation = Quaternion.LookRotation(direction);

            //�����ʂ��v�Z
            float speed = horizontal * acceleration * Time.deltaTime;

            //�󒆂ɕ����Ă��鎞�͈ړ��ʂ�␳����
            if(!controller.isGrounded)
            {
                speed *= airControl;
            }

            //��������
            horizontalSpeed += speed;

            //���x�����ȏ�Ȃ琧������
            if(Mathf.Abs(horizontalSpeed)>maxMoveSpeed)
            {
                horizontalSpeed = Mathf.Sign(horizontalSpeed) * maxMoveSpeed;
            }
        }
        //��������
        else
        {
            if(Mathf.Abs(horizontalSpeed)>0)
            {
                //�����ʂ��v�Z
                float speed = Mathf.Sign(horizontalSpeed) * friction * Time.deltaTime;

                //�󒆂ɕ����Ă��鎞�͈ړ��ʂ�␳����
                if (!controller.isGrounded)
                {
                    speed *= airControl;
                }

                //��������
                horizontalSpeed -= speed;

                //���x�����ȏ�Ȃ�~�߂�
                if(Mathf.Abs(horizontalSpeed)<friction*Time.deltaTime)
                {
                    horizontalSpeed = 0.0f;
                }
            }
        }
        ////�����ړ����x
        //horizontalSpeed = horizontal * maxMoveSpeed;

        //�n�ʂɐڒn���Ă��鎞
        if(controller.isGrounded)
        {
            //�A�j���[�^�[�Ƀp�����[�^�[�𑗐M
            animator.SetFloat("Speed", power);
        }
    }

    //�ړ��X�V����
    private void UpdateMovement()
    {
        //�A�j���[�^�[�Ƀp�����[�^�[�𑗐M
        animator.SetBool("Ground", controller.isGrounded);

        //�ړ��ʂ��v�Z
        //Vector3 move = new Vector3(horizontalSpeed, 0, 0);
        Vector3 move = new Vector3(horizontalSpeed, verticalSpeed, 0);

        //���̈ړ��ɒǏ]���鏈��
        if (activeFloor!=null)
        {
            //�O��̏�����ɂ����ʒu�ƍ���̏��̈ʒu���獡��̃O���[�o����̈ʒu���Z�o
            Vector3 newGlobalFloorPoint = activeFloor.TransformPoint(activeLocalFloorPoint);
            //�O��ƍ���̃O���[�o����̈ʒu�̍��������߂邱�Ƃŏ��̈ړ��ʂ��Z�o����
            Vector3 moveDistance = newGlobalFloorPoint - activeGlobalFloorPoint;
            //���݂̈ʒu�ɏ��̈ړ��ʂ����Z����
            controller.enabled = false;
            transform.position += moveDistance;
            controller.enabled = true;
        }

        //�󒆎��ԍX�V
        airFrame++;
        if(airFrame>2)
        {
            activeFloor = null;
        }

        //�ړ������i���̊֐�����OnControllerColliderHit�֐����Ă΂��j
        controller.Move(move * Time.deltaTime);

        //���̈ړ��ʎZ�o�̂��߁A���̊�ɂ����ʒu��ۑ�
        if(activeFloor!=null)
        {
            //�O���[�o����̈ʒu�Ə�����ɂ����ʒu��ۑ�
            activeGlobalFloorPoint = transform.position;
            activeLocalFloorPoint = activeFloor.InverseTransformPoint(transform.position);
        }
    }

    //�d�͏���
    private void UpdateGravity()
    {
        //�n�ʂɐڒn���Ă��鎞�̐����ړ����x�͈��̏d�͗�
        if(controller.isGrounded)
        {
            verticalSpeed = -gravity * Time.deltaTime;
        }
        //�󒆂ł͐����ړ����x�ɏd�͂����Z���Ă���
        else
        {
            verticalSpeed -= gravity * Time.deltaTime;
        }
        //�����ړ����x���ő嗎�����x�𒴂��Ȃ��悤�ɐ���
        verticalSpeed = Mathf.Max(verticalSpeed, -maxFallSpeed);
    }

    //�W�����v����
    private void UpdateJump()
    {
        //�n�ʂɐڒn���Ă����ԂŃW�����v�{�^���������Ɛ����ړ����x��ݒ肷��
        if(controller.isGrounded)
        {
            if(Input.GetButtonDown("Jump"))
            {
                verticalSpeed = jumpSpeed;

                //�A�j���[�^�[�Ƀp�����[�^�[�𑗐M
                animator.SetTrigger("Jump");
            }
        }
    }

    //�R�C��UI�\���X�V
    private void UpdateCoinText()
    {
        coinText.text = "x" + coinCount;
    }

    //�R�C���l������
    public void AddCoin(int amount)
    {
        coinCount += amount;

        //UI�\���X�V
        UpdateCoinText();
    }

    //�X�|�[��
    private void Spawn()
    {
        //�ړ��͂����Z�b�g
        verticalSpeed = 0.0f;
        horizontalSpeed = 0.0f;


        Warp(spawnPoint.position);
    }

    //���[�v
    public void Warp(Vector3 position)
    {
        // ���L�����N�^�[�R���g���[���[�g�p���ɒ��ڈʒu��ύX����ꍇ��
        //  �@�L�����N�^�[�R���g���[���[�𖳌������Ă���ݒ肷��K�v������
        controller.enabled = false;
        transform.position = position;
        controller.enabled = true;

    }

    //���S����
    public void Death()
    {
        //���C�t�����炷
        life--;
        if (life <= 0)
        {
            //���݂̃V�[�����ă��[�h
            SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        }
        //UI�\���X�V
        UpdateLifeText();

        //�X�|�[��
        Spawn();
    }

    private void UpdateLifeText()
    {
        lifeText.text = "x" + life;
    }

    //�X�|�[���|�C���g�ݒ�
    public void SetSpawnPoint(Transform transform)
    {
        spawnPoint = transform;
    }

    //CharacterController�̈ړ��ŃR���C�_�[�ƏՓ˂����ۂɌĂ΂��
    private void OnControllerColliderHit(ControllerColliderHit hit)
    {
        //���̂�����
        PushRigidbody(hit);

        //�V��`�F�b�N
        CeilingCheck(hit);

        //�f�X�g���K�[�Ƃ̏Փˏ���
        CollisionDeathTrigger(hit);

        //���`�F�b�N
        FloorCheck(hit);
    }

    //�V��`�F�b�N
    private void CeilingCheck(ControllerColliderHit hit)
    {
        //���������ʂ̌�������������������V��
        if(hit.normal.y<-0.8f)
        {
            //������ɗ͂������Ă���ꍇ�̓��Z�b�g
            if(verticalSpeed>0.0f)
            {
                verticalSpeed = 0.0f;
            }
        }
    }

    //�ڐG�������̂�����
    private void PushRigidbody(ControllerColliderHit hit)
    {
        Rigidbody body = hit.collider.attachedRigidbody;

        //���W�b�h�{�f�B�`�F�b�N
        if(body==null||body.isKinematic)
        {
            return;
        }
        //�������ɂ͉����Ȃ�
        if(hit.moveDirection.y<-0.3f)
        {
            return;
        }
        //�ړ��������牟���������Z�o����(�㉺�����ɂ͉����Ȃ�)
        Vector3 pushDir = new Vector3(hit.moveDirection.x, 0, hit.moveDirection.z);
        Vector3 pushVelocity = pushDir * pushPower;
        pushVelocity.y = body.velocity.y;
        //�����͂�ݒ�
        body.velocity = pushVelocity;
    }

    //�f�X�g���K�[�Ƃ̏Փˏ���
    private void CollisionDeathTrigger(ControllerColliderHit hit)
    {
        //�f�X�g���K�[�ƏՓ˂����玀�S����
        DeathTrigger deathTrigger = hit.gameObject.GetComponent<DeathTrigger>();
        if(deathTrigger!=null)
        {
            Death();
        }
    }

    //���`�F�b�N
    private void FloorCheck(ControllerColliderHit hit)
    {
        //���������ʂ̌�����������������珰
        if(hit.normal.y>0.9f)
        {
            //����ł��鏰���L��
            activeFloor = hit.collider.transform;
            //�󒆎��Ԃ����Z�b�g
            airFrame = 0;
        }
    }

}
