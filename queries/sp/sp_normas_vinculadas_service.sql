DELIMITER //


CREATE PROCEDURE sp_insertar_norma_vinculada(
    IN p_id_norma_principal INT,
    IN p_path_name varchar(101)
)
BEGIN
    INSERT INTO normas_vinculadas (id_norma_principal, path_name)
    VALUES (p_id_norma_principal, p_path_name);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //


DELIMITER ;
