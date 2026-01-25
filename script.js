async function loadInventory() {
  const res = await fetch("inventory.json");
  const data = await res.json();

  const table = document.getElementById("table");
  table.innerHTML = `
    <tr>
      <th>ID</th><th>Name</th><th>Qty</th><th>Price</th>
    </tr>
  `;

  data.forEach(p => {
    table.innerHTML += `
      <tr>
        <td>${p.id}</td>
        <td>${p.name}</td>
        <td>${p.quantity}</td>
        <td>${p.price}</td>
      </tr>
    `;
  });
}

setInterval(loadInventory, 2000);
loadInventory();
