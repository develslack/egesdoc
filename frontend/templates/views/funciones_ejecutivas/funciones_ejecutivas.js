console.clear();
console.log("✅ Módulo Funciones Ejecutivas Iniciado de forma nativa.");

// CORRECCIÓN: Usamos el objeto window para evitar el SyntaxError de redeteclaración con let
window.dTable = window.dTable || null;

(async () => {
  try {
    // ================================================================================================================= //
    // 1. CARGA DE LA TABLA MAESTRA DESDE EL BACKEND EN C
    // ================================================================================================================= //
    const response = await fetch(window.API_BASE_URL +"/funciones_ejecutivas/list");
    const registros = await response.json();

    const tableBody = document.getElementById("funcionesEjecutivasTableBody");
    const alertInfo = document.getElementById("alert-info-funciones_ejecutivas");
    tableBody.innerHTML = "";

    let count = 0;

    registros.forEach((reg) => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <td class="text-center">${reg.nivel}</td>
        <td class="text-center">${reg.cant_ur}</td>
        <td class="text-center">$${reg.valor_ur}</td>
        <td class="text-center">$${reg.monto}</td>
        <td class="text-center">${reg.norma_regulatoria}</td>
        <td class="text-center">${reg.f_entrada_vigencia}</td>
        <td class="text-center">${reg.mes}</td>
        <td class="text-center">${reg.anio}</td>
        <td class="text-center">
          <button class="btn btn-default btn-sm btn-editar-funcion_ejecutiva"
                  data-id="${reg.id}"
                  data-nivel="${reg.nivel}"
                  data-cant_ur="${reg.cant_ur}"
                  data-valor_ur="${reg.valor_ur}"
                  data-monto="${reg.monto}"
                  data-norma_regulatoria="${reg.norma_regulatoria}"
                  data-f_entrada_vigencia="${reg.f_entrada_vigencia}"
                  data-mes="${reg.mes}"
                  data-anio="${reg.anio}">
            <span class="glyphicon glyphicon-edit"></span> Editar
          </button>
        </td>`;

      tableBody.appendChild(row);
      count++;
    });

    if (alertInfo) {
        alertInfo.innerHTML = `<div class="alert alert-info">
                                <span class="glyphicon glyphicon-option-vertical" aria-hidden="true"></span> <strong>Cantidad de Registros: </strong> ${count}
                               </div><hr>`;
    }

    if ($.fn.DataTable.isDataTable("#funcionesEjecutivasTable")) {
      $("#funcionesEjecutivasTable").DataTable().destroy();
    }

    dTable = $('#funcionesEjecutivasTable').DataTable({
        "order": [[0, "asc"]],
        "responsive":     true,
        "scrollY":        "300px",
        "scrollX":        true,
        "scrollCollapse": true,
        "paging":         true,
        "deferRender":    true,
        "retrieve":       true,
        "dom": '<"row"<"col-sm-12"l>><"row"<"col-sm-12"Bf>>rt<"row"<"col-sm-12"ip>>',
        buttons: [
            { extend: 'excel', text: '<i class="glyphicon glyphicon-list-alt"></i> Export Excel', exportOptions: { columns: ':visible'} },
            { extend: 'csv', text: '<i class="glyphicon glyphicon-align-justify"></i> Export CSV', exportOptions: { columns: ':visible'} },
            { extend: 'pdf', text: '<i class="glyphicon glyphicon-file"></i> Export PDF', exportOptions: { columns: ':visible'} },
            {
                extend: 'print',
                text: '<i class="glyphicon glyphicon-print"></i> Imprimir',
                customize: function ( win ) {
                    $(win.document.body).css( 'font-size', '8pt' );$(win.document.body).find( 'table' ).addClass( 'compact' ).css( 'font-size', 'inherit' );
                },
                autoPrint: false,
                exportOptions: { columns: ':visible' }
            },
            'colvis'
        ],
        "language": {
            "lengthMenu": "Mostrar _MENU_ registros por pagina",
            "info": "Mostrando pagina _PAGE_ de _PAGES_",
            "search": "Buscar:",
            "zeroRecords":    "No se encontraron registros coincidentes",
            "paginate": { "next": "Siguiente", "previous": "Anterior" }
        }
    });

    // ================================================================================================================= //
    // 2. PREPARACIÓN DEL MODAL USANDO EL SISTEMA DEL DASHBOARD
    // ================================================================================================================= //
    // Usamos el fetch nativo del servidor para rellenar el div de abajo sin pisar el dashboard central
    const contenedorModal = document.getElementById("contenedor-modal-maestro-funciones_ejecutivas");
    if (contenedorModal) {
        const respModal = await fetch(window.VIEWS_PATH + "/funciones_ejecutivas/nueva_funcion_ejecutiva.html");
        contenedorModal.innerHTML = await respModal.text();
    }

    // ================================================================================================================= //
    // 3. CAPTURA SEGURO DE EVENTOS (DELEGACIÓN DE EVENTOS JQUERY)
    // ================================================================================================================= //

    // Evento Añadir (Alta)
    $(document).off("click", "#add-funcion_ejecutiva-form").on("click", "#add-funcion_ejecutiva-form", function(e) {
        e.preventDefault();
        prepararYMostrarModal("alta");
    });

    // Evento Editar (Fila de la tabla)
    $(document).off("click", ".btn-editar-funcion_ejecutiva").on("click", ".btn-editar-funcion_ejecutiva", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nivel: $(this).attr("data-nivel"),
            cant_ur: $(this).attr("data-cant_ur"),
            valor_ur: $(this).attr("data-valor_ur"),
            monto: $(this).attr("data-monto"),
            norma_regulatoria: $(this).attr("data-norma_regulatoria"),
            f_entrada_vigencia: $(this).attr("data-f_entrada_vigencia"),
            mes: $(this).attr("data-mes"),
            anio: $(this).attr("data-anio")
        };
        prepararYMostrarModal("edicion", datos);
    });

  } catch (error) {
    console.error("💥 Error general en el módulo Funciones Ejecutivas:", error);
  }
})();

// ===================================================================================================================== //
// FUNCIÓN QUE CONSTRUYE EL FORMULARIO INTERNO Y MUESTRA EL MODAL FLOTANTE
// ===================================================================================================================== //
function prepararYMostrarModal(modo, datos = null) {
    const $modal =$("#myModal-funciones_ejecutivas");
    if ($modal.length === 0) return;

    // Seteamos título del modal flotante
    $modal.find(".modal-title").text(modo === "alta" ? "Añadir Función Ejecutiva" : "Editar Función Ejecutiva");

    // Seteamos campos en el body vacío
    $modal.find(".modal-body").html(`

        <form id="form-modal-funcion_ejecutiva">
            <input type="hidden" id="modal-funcion_ejecutiva-id" value="${datos ? datos.id : ''}">
            <div class="form-group">
                <label>Nivel:</label>
                <input type="text" class="form-control" id="modal-funcion_ejecutiva-nivel" value="${datos ? datos.nivel : ''}" required>
            </div>
            <div class="form-group">
                <label>Cantidad UR:</label>
                <input type="text" class="form-control" id="modal-funcion_ejecutiva-cant_ur" value="${datos ? datos.cant_ur : ''}" required>
            </div>
            <div class="form-group">
                <label>Valor UR:</label>
                <input type="text" class="form-control" id="modal-funcion_ejecutiva-valor_ur" value="${datos ? datos.valor_ur : ''}" required>
            </div>
            <div class="form-group">
                <label>Monto:</label>
                <input type="text" class="form-control" id="modal-funcion_ejecutiva-monto" value="${datos ? datos.monto : ''}" required>
            </div>
            <div class="form-group">
                <label>Norma Regulatoria:</label>
                <input type="text" class="form-control" id="modal-funcion_ejecutiva-norma_regulatoria" value="${datos ? datos.norma_regulatoria : ''}" required>
            </div>
            <div class="form-group">
                <label>Fecha Entrada Vigencia:</label>
                <input type="date" class="form-control" id="modal-funcion_ejecutiva-f_entrada_vigencia" value="${datos ? datos.f_entrada_vigencia : ''}" required>
            </div>
            <div class="form-group">
                <label>Mes:</label>
                <input type="text" class="form-control" id="modal-funcion_ejecutiva-mes" value="${datos ? datos.mes : ''}" required>
            </div>
            <div class="form-group">
                <label>Año:</label>
                <!-- CORRECCIÓN APLICADA: id cambiado a "anio" -->
                <input type="text" class="form-control" id="modal-funcion_ejecutiva-anio" value="${datos ? datos.anio : ''}" required>
            </div>
            <button type="submit" id="btn-submit-oculto" style="display:none;"></button>
        </form>
    `);

    // Seteamos barra de estado y botones en el footer vacío
    $modal.find(".modal-footer").html(`
        <div id="modal-status-message" style="margin-bottom: 10px; text-align: left;"></div>
        <div class="text-right">
            <button type="button" class="btn btn-danger" data-dismiss="modal">
                <span class="glyphicon glyphicon-remove-circle" aria-hidden="true"></span> Cerrar</button>
            <button type="button" class="btn btn-success" id="btn-modal-guardar">
                <span class="glyphicon glyphicon-ok" aria-hidden="true"></span> Guardar</button>
        </div>
    `);

    // Hacemos que aparezca de forma flotante sobre dashboard.html
    $modal.modal("show");

    // =========================================================================
    // NUEVO CÓDIGO: CÁLCULO AUTOMÁTICO DE MONTO
    // =========================================================================
    const inputCantUr = document.getElementById("modal-funcion_ejecutiva-cant_ur");
    const inputValorUr = document.getElementById("modal-funcion_ejecutiva-valor_ur");
    const inputMonto = document.getElementById("modal-funcion_ejecutiva-monto");

    inputMonto.addEventListener("focus", function() {
        const cant = parseFloat(inputCantUr.value.replace(',', '.')) || 0;
        const valor = parseFloat(inputValorUr.value.replace(',', '.')) || 0;
        const resultado = (cant * valor).toFixed(2);
        this.value = resultado;
    });
    // =========================================================================

    // Vinculamos el botón verde del footer con la validación del form
    document.getElementById("btn-modal-guardar").onclick = () => {
        document.getElementById("btn-submit-oculto").click();
    };

    // Procesamiento del submit al backend en C
    document.getElementById("form-modal-funcion_ejecutiva").onsubmit = async (e) => {
        e.preventDefault();

        const statusContainer = document.getElementById("modal-status-message");
        const id = document.getElementById("modal-funcion_ejecutiva-id").value;

        const payload = {
            nivel: document.getElementById("modal-funcion_ejecutiva-nivel").value,
            cant_ur: document.getElementById("modal-funcion_ejecutiva-cant_ur").value,
            valor_ur: document.getElementById("modal-funcion_ejecutiva-valor_ur").value,
            monto: document.getElementById("modal-funcion_ejecutiva-monto").value,
            norma_regulatoria: document.getElementById("modal-funcion_ejecutiva-norma_regulatoria").value,
            f_entrada_vigencia: document.getElementById("modal-funcion_ejecutiva-f_entrada_vigencia").value,
            mes: document.getElementById("modal-funcion_ejecutiva-mes").value,
            anio: document.getElementById("modal-funcion_ejecutiva-anio").value
        };

        if(id == ""){

            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/funciones_ejecutivas/add", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `nivel=${encodeURIComponent(payload.nivel)}&cant_ur=${encodeURIComponent(payload.cant_ur)}&valor_ur=${encodeURIComponent(payload.valor_ur)}&monto=${encodeURIComponent(payload.monto)}&norma_regulatoria=${encodeURIComponent(payload.norma_regulatoria)}&f_entrada_vigencia=${encodeURIComponent(payload.f_entrada_vigencia)}&mes=${encodeURIComponent(payload.mes)}&anio=${encodeURIComponent(payload.anio)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/funciones_ejecutivas/funciones_ejecutivas.html");
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            }
            catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> Ha ocurrido un problema al procesar la solicitud.</span>`;
            }

        }
        if(id != ""){

            console.log(payload);
            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/funciones_ejecutivas/edit", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `id=${encodeURIComponent(id)}&nivel=${encodeURIComponent(payload.nivel)}&cant_ur=${encodeURIComponent(payload.cant_ur)}&valor_ur=${encodeURIComponent(payload.valor_ur)}&monto=${encodeURIComponent(payload.monto)}&norma_regulatoria=${encodeURIComponent(payload.norma_regulatoria)}&f_entrada_vigencia=${encodeURIComponent(payload.f_entrada_vigencia)}&mes=${encodeURIComponent(payload.mes)}&anio=${encodeURIComponent(payload.anio)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/funciones_ejecutivas/funciones_ejecutivas.html");
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            }
            catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> Ha ocurrido un problema al procesar la solicitud.</span>`;
            }

        }

    };
}
